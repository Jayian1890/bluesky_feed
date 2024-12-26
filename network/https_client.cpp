//
// Created by jayian on 11/26/24.
// Copyright (c) 2024 Interlaced Pixel. All rights reserved.
//

#include "https_client.h"
#include <sstream>
#include "../cpp-httplib/httplib.h"
#include "../tools/logging.hpp"

// Setters
void HTTPSClient::setHost(const std::string_view h) {
    if (h.find("https://") == 0) {
        host = h.substr(8);
    } else {
        host = h;
    }

    // Remove any trailing slashes
    if (!host.empty() && host.back() == '/') {
        host.pop_back();
    }

    Logging::debug("Host set to: " + host);
}

void HTTPSClient::setEndpoint(const std::string_view ep) { endpoint = ep; }
void HTTPSClient::setBearerToken(const std::string_view token) { bearerToken = token; }
void HTTPSClient::addQueryParam(const std::string_view key, const std::string_view value) {
    queryParams[std::string(key)] = value;
}

// Construct the full URL including query parameters
std::string HTTPSClient::constructUrl() const {
    std::ostringstream urlStream;

    // Ensure the URL starts with https://
    urlStream << "https://" << host << endpoint;

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

    Logging::debug("Constructed URL: " + urlStream.str());
    return urlStream.str();
}

// Perform a GET request and return JSON
nlohmann::json HTTPSClient::get() const {
    if (host.empty()) {
        Logging::error("HTTPSClient::get()", "host is empty");
        return {};
    }

    httplib::SSLClient client(host, 443);
    client.set_follow_location(true); // Follow redirects automatically

    // Set headers
    httplib::Headers headers;
    headers.insert({"Accept", "application/json"});
    if (!bearerToken.empty()) {
        headers.insert({"Authorization", "Bearer " + bearerToken});
    }

    // Perform the GET request
    auto res = client.Get(constructUrl(), headers);

    // Check response status
    if (!res || res->status != 200) {
        Logging::error("HTTP GET failed with status: " + (res ? std::to_string(res->status) : "No response"));
        if (res) {
            Logging::error("Response body: " + res->body);
        }
        for (const auto& [key, value] : queryParams) {
            Logging::debug("Query Param: " + key + " = " + value);
        }
        return {};
    }

    // Parse JSON response body
    const auto& body = res->body;
    return nlohmann::json::parse(body.data(), body.data() + body.size());
}