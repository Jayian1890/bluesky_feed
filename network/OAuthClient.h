//
// Created by jayian on 11/26/24.
// Copyright (c) 2024 Interlaced Pixel. All rights reserved.
//

#ifndef OAUTHCLIENT_H
#define OAUTHCLIENT_H

#pragma once

#include <iostream>
#include <string>
#include <map>
#include <curl/curl.h>
#include "../tools/JSON.h"
#include "../config/Settings.h"

class OAuthClient {
    std::string clientId;
    std::string clientSecret;
    std::string authEndpoint;
    std::string tokenEndpoint;
    std::string redirectUri;
    std::string accessToken;

    // Static callback for libcurl
    static size_t writeCallback(void* contents, const size_t size, const size_t nmemb, std::string* response) {
        const size_t totalSize = size * nmemb;
        response->append(static_cast<char*>(contents), totalSize);
        return totalSize;
    }

    // Helper function for making HTTP POST requests
    static std::string postRequest(const std::string& url, const std::map<std::string, std::string>& postData) {
        CURL* curl = curl_easy_init();
        if (!curl) {
            throw std::runtime_error("Failed to initialize CURL");
        }

        std::string response;
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");

        // Construct POST fields
        std::string postFields;
        for (const auto& [key, value] : postData) {
            if (!postFields.empty()) {
                postFields += "&";
            }
            postFields += key + "=" + curl_easy_escape(curl, value.c_str(), value.length());
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        if (const CURLcode res = curl_easy_perform(curl); res != CURLE_OK) {
            curl_easy_cleanup(curl);
            curl_slist_free_all(headers);
            throw std::runtime_error(std::string("CURL error: ") + curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
        return response;
    }

public:
    OAuthClient(std::string  clientId, std::string  clientSecret,
                std::string  authEndpoint, std::string  tokenEndpoint,
                std::string  redirectUri)
        : clientId(std::move(clientId)), clientSecret(std::move(clientSecret)),
          authEndpoint(std::move(authEndpoint)), tokenEndpoint(std::move(tokenEndpoint)), redirectUri(std::move(redirectUri)) {}

    // Get authorization URL
    [[nodiscard]] std::string getAuthUrl(const std::string& scope, const std::string& state) const {
        return authEndpoint + "?response_type=code&client_id=" + clientId +
               "&redirect_uri=" + redirectUri + "&scope=" + scope + "&state=" + state;
    }

    // Exchange authorization code for an access token
    void fetchAccessToken(const std::string& authCode) {
        const std::map<std::string, std::string> postData = {
            {"grant_type", "authorization_code"},
            {"code", authCode},
            {"redirect_uri", redirectUri},
            {"client_id", clientId},
            {"client_secret", clientSecret}
        };

        const std::string response = postRequest(tokenEndpoint, postData);

        JSON json;
        json.parse(response);
        accessToken = json.get("access_token");

        if (accessToken.empty()) {
            throw std::runtime_error("Failed to fetch access token");
        }
    }

    // Get the stored access token
    [[nodiscard]] std::string getAccessToken() const {
        return accessToken;
    }

    static void authenticate() {
        try {
            auto& settings = Settings::getInstance();

            const std::string clientId = settings.get("client_id");
            const std::string clientSecret = settings.get("client_secret");
            const std::string authEndpoint = settings.get("auth_endpoint");
            const std::string tokenEndpoint = settings.get("token_endpoint");
            const std::string redirectUri = settings.get("redirect_uri");

            OAuthClient oauth(clientId, clientSecret, authEndpoint, tokenEndpoint, redirectUri);

            // Step 1: Generate authorization URL
            std::string authUrl = oauth.getAuthUrl("read_profile write_profile", "random_state");
            std::cout << "Visit this URL to authorize: " << authUrl << std::endl;

            // Step 2: After the user authorizes, they will be redirected with a code
            std::string authCode;
            std::cout << "Enter the authorization code: ";
            std::cin >> authCode;

            // Step 3: Exchange the code for an access token
            oauth.fetchAccessToken(authCode);

            // Step 4: Use the access token
            auto accessToken = oauth.getAccessToken();
            settings.set("accessToken", accessToken);
            std::cout << "Access Token: " << accessToken << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
};

#endif //OAUTHCLIENT_H