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

// Custom HTTP exception
class HTTPException final : public std::exception {
    std::string message;

public:
    explicit HTTPException(std::string msg) : message(std::move(msg)) {}

    [[nodiscard]] const char* what() const noexcept override {
        return message.c_str();
    }
};

class HTTPSClient {
    std::string host;
    std::string endpoint;
    std::string bearerToken;
    std::map<std::string, std::string, std::less<>> queryParams;

    // Construct the full URL including query parameters
    [[nodiscard]] std::string constructUrl() const;

public:
    // Setters
    void setHost(std::string_view h);
    void setEndpoint(std::string_view ep);
    void setBearerToken(std::string_view token);
    void addQueryParam(std::string_view key, std::string_view value);

    // Perform a GET request and return JSON
    [[nodiscard]] nlohmann::json get() const;
};

#endif // HTTPSCLIENT_H