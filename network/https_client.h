//
// Created by jayian on 11/26/24.
// Copyright (c) 2024 Interlaced Pixel. All rights reserved.
//

#ifndef HTTPSCLIENT_H
#define HTTPSCLIENT_H

#pragma once

#include <map>
#include <string>
#include "../nlohmann/json.hpp"

class HTTPSClient {
    std::string host;
    std::string endpoint;
    std::string bearerToken;
    std::map<std::string, std::string> queryParams;

    // Static callback to store response data
    static size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* response);

    // Construct the full URL including query parameters
    [[nodiscard]] std::string constructUrl() const;

public:
    // Singleton instance getter
    static HTTPSClient* getInstance();

    // Constructor
    HTTPSClient();

    // Setters
    void setHost(const std::string& h);
    void setEndpoint(const std::string& ep);
    void setBearerToken(const std::string& token);
    void addQueryParam(const std::string& key, const std::string& value);

    // Perform a GET request and return JSON
    [[nodiscard]] nlohmann::json get() const;
};


#endif //HTTPSCLIENT_H
