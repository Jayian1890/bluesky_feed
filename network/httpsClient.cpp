//
// Created by jayian on 11/26/24.
// Copyright (c) 2024 Interlaced Pixel. All rights reserved.
//

#include <stdexcept>
#include <curl/curl.h>
#include "httpsClient.h"

// Static callback to store response data
size_t httpsClient::writeCallback(void* contents, const size_t size, const size_t nmemb, std::string* response) {
    const size_t totalSize = size * nmemb;
    response->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

// Constructor
httpsClient::httpsClient() = default;

// Singleton instance getter
httpsClient* httpsClient::getInstance() {
    static httpsClient instance;
    return &instance;
}

// Setters
void httpsClient::setHost(const std::string& h) { host = h; }
void httpsClient::setEndpoint(const std::string& ep) { endpoint = ep; }
void httpsClient::setBearerToken(const std::string& token) { bearerToken = token; }
void httpsClient::addQueryParam(const std::string& key, const std::string& value) {
    queryParams[key] = value;
}

// Construct the full URL including query parameters
std::string httpsClient::constructUrl() const {
    std::string url = host + endpoint;
    if (!queryParams.empty()) {
        url += "?";
        for (auto it = queryParams.begin(); it != queryParams.end(); ++it) {
            if (it != queryParams.begin()) {
                url += "&";
            }
            url += it->first + "=" + it->second;
        }
    }
    return url;
}

// Perform a GET request and return JSON
JSON httpsClient::get() const {
    CURL* curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Failed to initialize libcurl");
    }

    const std::string url = constructUrl();
    std::string responseData;
    struct curl_slist* headers = nullptr;

    // Set up headers
    if (!bearerToken.empty()) {
        const std::string authHeader = "Authorization: Bearer " + bearerToken;
        headers = curl_slist_append(headers, authHeader.c_str());
    }
    headers = curl_slist_append(headers, "Accept: application/json");

    // Set CURL options
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

    // Perform the request
    if (const CURLcode res = curl_easy_perform(curl); res != CURLE_OK) {
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
        throw std::runtime_error(std::string("CURL error: ") + curl_easy_strerror(res));
    }

    // Clean up
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    // Parse JSON response
    JSON json;
    json.parse(responseData);
    return json;
}