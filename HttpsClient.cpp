#pragma once

#include <iostream>
#include <string>
#include <stdexcept>
#include <curl/curl.h>
#include "json.cpp"

class HttpsClient {
private:
    std::string host;
    std::string endpoint;
    std::string bearerToken;
    std::map<std::string, std::string> queryParams;

    // Static callback to store response data
    static size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* response) {
        size_t totalSize = size * nmemb;
        response->append(static_cast<char *>(contents), totalSize);
        return totalSize;
    }

public:
    static HttpsClient* getInstance() {
        static HttpsClient instance;
        return &instance;
    }

    HttpsClient() = default;

    void setHost(const std::string& h) { host = h; }
    void setEndpoint(const std::string& ep) { endpoint = ep; }
    void setBearerToken(const std::string& token) { bearerToken = token; }

    void addQueryParam(const std::string& key, const std::string& value) {
        queryParams[key] = value;
    }

    std::string constructUrl() const {
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

    JSON get() const {
        CURL* curl = curl_easy_init();
        if (!curl) {
            throw std::runtime_error("Failed to initialize libcurl");
        }

        const std::string url = constructUrl();
        std::string responseData;
        struct curl_slist* headers = nullptr;

        // Set up headers
        if (!bearerToken.empty()) {
            std::string authHeader = "Authorization: Bearer " + bearerToken;
            headers = curl_slist_append(headers, authHeader.c_str());
        }

        headers = curl_slist_append(headers, "Accept: application/json");

        // Set CURL options
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

        // Perform the request
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
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
};