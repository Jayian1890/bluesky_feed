//
// Created by jayian on 11/26/24.
// Copyright (c) 2024 Interlaced Pixel. All rights reserved.
//

#include "https_client.h"
#include <sstream>
#include "../cpp-httplib/httplib.h"

// Setters
void HTTPSClient::setHost(const std::string_view h) { host = h; }
void HTTPSClient::setEndpoint(const std::string_view ep) { endpoint = ep; }
void HTTPSClient::setBearerToken(const std::string_view token) { bearerToken = token; }
void HTTPSClient::addQueryParam(const std::string_view key, const std::string_view value) {
    queryParams[std::string(key)] = value;  // Convert to std::string for map compatibility
}

// Construct the full URL including query parameters
std::string HTTPSClient::constructUrl() const {
    std::ostringstream urlStream;

    // Ensure the host and endpoint are combined correctly.
    // Assuming `endpoint` is relative, prepend `host` if needed.
    urlStream << host << endpoint;
    if (!queryParams.empty()) {
        urlStream << "?";
        for (auto it = queryParams.begin(); it != queryParams.end(); ++it) {
            if (it != queryParams.begin()) {
                urlStream << "&";
            }
            // URL encode the query params
            urlStream << httplib::detail::encode_url(it->first) << "="
                      << httplib::detail::encode_url(it->second);
        }
    }
    return urlStream.str();
}

// Perform a GET request and return JSON
nlohmann::json HTTPSClient::get() const {
    if (host.empty()) {
        throw HTTPException("Host not set for HTTPSClient");
    }

    httplib::SSLClient client(host);  // Use SSL client for HTTPS
    client.set_follow_location(true); // Follow redirects automatically

    const std::string constructedUrl = constructUrl();

    // Set headers
    httplib::Headers headers;
    headers.insert({"Accept", "application/json"});
    if (!bearerToken.empty()) {
        headers.insert({"Authorization", "Bearer " + bearerToken});
    }

    // Perform the GET request
    auto res = client.Get(constructedUrl, headers);

    // Check response status
    if (!res || res->status != 200) {
        //throw HTTPException("HTTP GET failed with status: " + (res ? std::to_string(res->status) : "No response"));
        return nullptr;
    }

    // Parse JSON response body
    const auto& body = res->body;
    return nlohmann::json::parse(body.data(), body.data() + body.size());
}