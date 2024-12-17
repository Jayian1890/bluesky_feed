//
// Created by jayian on 12/05/24.
// Copyright (c) 2024 Interlaced Pixel. All rights reserved.
//

#ifndef REDIRECTURIHANDLER_H
#define REDIRECTURIHANDLER_H

#pragma once

#include <string>
#include <iostream>
#include "../cpp-httplib/httplib.h" // Lightweight HTTP server library

class RedirectUriHandler {
    std::string redirectUri;
    std::string receivedCode;
    bool isCodeReceived = false;

    static std::string getPathFromUri(const std::string &uri) {
        const auto pos = uri.find('?');
        return pos == std::string::npos ? uri : uri.substr(0, pos);
    }

public:
    explicit RedirectUriHandler(std::string uri) : redirectUri(std::move(uri)) {}

    void startServer() {
        httplib::Server svr;

        svr.Get(getPathFromUri(redirectUri), [this](const httplib::Request &req, httplib::Response &res) {
            if (req.has_param("code")) {
                receivedCode = req.get_param_value("code");
                isCodeReceived = true;
                res.set_content("Authorization code received. You can close this tab.", "text/plain");
            } else {
                res.set_content("Authorization code not found in the request.", "text/plain");
            }
        });

        const auto uriParts = parseUri();
        const auto port = uriParts.port.empty() ? 80 : std::stoi(uriParts.port);

        std::cout << "Listening on port " << port << " for redirect..." << std::endl;
        svr.listen(uriParts.host, port);
    }

    [[nodiscard]] std::string getAuthorizationCode() const {
        if (!isCodeReceived) {
            throw std::runtime_error("Authorization code has not been received yet.");
        }
        return receivedCode;
    }

private:
    struct UriParts {
        std::string scheme;
        std::string host;
        std::string port;
        std::string path;
    };

    [[nodiscard]] UriParts parseUri() const {
        UriParts parts;
        const auto posScheme = redirectUri.find("://");
        if (posScheme == std::string::npos) throw std::invalid_argument("Invalid URI");

        parts.scheme = redirectUri.substr(0, posScheme);
        const auto posHostStart = posScheme + 3;

        const auto posPort = redirectUri.find(':', posHostStart);
        const auto posPath = redirectUri.find('/', posHostStart);

        if (posPort != std::string::npos && posPort < posPath) {
            parts.host = redirectUri.substr(posHostStart, posPort - posHostStart);
            const auto posPortStart = posPort + 1;
            parts.port = redirectUri.substr(posPortStart, posPath - posPortStart);
        } else {
            parts.host = redirectUri.substr(posHostStart, posPath - posHostStart);
        }

        parts.path = redirectUri.substr(posPath);
        return parts;
    }
};

#endif // REDIRECTURIHANDLER_H