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
#include <unordered_map>
#include <curl/curl.h>
#include "../nlohmann/json.hpp"
#include "../config/Settings.h"
#include "../tools/url_encoder.h"

class OAuthClient {
    // Member variables
    std::string clientId;       // OAuth application client ID used for identification
    std::string clientSecret;   // OAuth application client secret for authentication
    std::string authEndpoint;   // Authorization endpoint URL
    std::string tokenEndpoint;  // Token endpoint URL
    std::string redirectUri;    // Redirect URI for the OAuth application
    std::string accessToken;    // Stored access token obtained from the OAuth server

    // Static callback for libcurl to handle incoming response data
    static size_t writeCallback(void *contents, const size_t size, const size_t nmemb, std::string *response) {
        const size_t totalSize = size * nmemb;
        response->append(static_cast<char *>(contents), totalSize);
        return totalSize;
    }

    // Helper function for making HTTP POST requests using libcurl
    static std::string postRequest(const std::string &url, const std::map<std::string, std::string> &postData) {
        CURL *curl = curl_easy_init();
        if (!curl) {
            throw std::runtime_error("Failed to initialize CURL");
        }

        std::string response;
        curl_slist *headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");

        // Construct POST fields
        std::string postFields;
        for (const auto &[key, value]: postData) {
            if (!postFields.empty()) {
                postFields += "&";
            }

            // Ensure the string length fits within an int
            if (value.length() > static_cast<size_t>(std::numeric_limits<int>::max())) {
                curl_easy_cleanup(curl);
                curl_slist_free_all(headers);
                throw std::overflow_error("Value length exceeds maximum supported size for CURL");
            }

            postFields += key + "=" + curl_easy_escape(curl, value.c_str(), static_cast<int>(value.length()));
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
    // Constructor to initialize OAuthClient with client credentials, endpoints, and redirect URI
    OAuthClient(std::string clientId, std::string clientSecret,
                std::string authEndpoint, std::string tokenEndpoint,
                std::string redirectUri)
        : clientId(std::move(clientId)), clientSecret(std::move(clientSecret)),
          authEndpoint(std::move(authEndpoint)), tokenEndpoint(std::move(tokenEndpoint)),
          redirectUri(std::move(redirectUri)) {
    }

    // Generate the OAuth authorization URL with scope and state
    [[nodiscard]] std::string getAuthUrl(const std::string &scope, const std::string &state) const {
        return authEndpoint + "?response_type=code&client_id=" + clientId +
               "&redirect_uri=" + redirectUri + "&scope=" + URLEncoder::encodeComponent(scope) + "&state=" + state;
    }

    // Exchange an authorization code for an access token
    void fetchAccessToken(const std::string &authCode) {
        const std::map<std::string, std::string> postData = {
            {"grant_type", "authorization_code"},
            {"code", authCode},
            {"redirect_uri", redirectUri},
            {"client_id", clientId},
            {"client_secret", clientSecret}
        };

        const std::string response = postRequest(tokenEndpoint, postData);

        auto json = nlohmann::json::parse(response);
        accessToken = json.at("access_token").get<std::string>();

        if (accessToken.empty()) {
            throw std::runtime_error("Failed to fetch access token");
        }
    }

    // Get the stored access token
    [[nodiscard]] std::string getAccessToken() const {
        return accessToken;
    }

    // Generate default OAuth client metadata information
    static std::unordered_map<std::string, std::string> generateOAuthClientInfo() {
        std::unordered_map<std::string, std::string> clientInfo;

        clientInfo["client_id"] = "https://example.com/client-metadata.json";
        clientInfo["client_name"] = "Example atproto Browser App";
        clientInfo["client_uri"] = "https://example.com";
        clientInfo["logo_uri"] = "https://example.com/logo.png";
        clientInfo["tos_uri"] = "https://example.com/tos";
        clientInfo["policy_uri"] = "https://example.com/policy";

        clientInfo["redirect_uris"] = "[\"https://example.com/callback\"]";
        clientInfo["scope"] = "atproto";
        clientInfo["grant_types"] = R"(["authorization_code", "refresh_token"])";
        clientInfo["response_types"] = "[\"code\"]";

        clientInfo["token_endpoint_auth_method"] = "none";
        clientInfo["application_type"] = "web";
        clientInfo["dpop_bound_access_tokens"] = "true";

        return clientInfo;
    }

    // Generate client-metadata.json using default metadata
    static void generateClientMetadata() {
        const auto clientInfo = generateOAuthClientInfo();

        const auto metadataSettings = Settings::createInstance("client-metadata.json", clientInfo);

        for (const auto &[key, defaultValue]: clientInfo) {
            if (!metadataSettings->hasKey(key) || metadataSettings->get(key).empty()) {
                metadataSettings->set(key, defaultValue);
                std::cout << "Setting \"" << key << "\" with default value \"" << defaultValue << "\"" << std::endl;
            }
        }
        const auto fullpath = Settings::getAbsolutePath("client-metadata.json");
        std::cout << "[OAuthClient] client metadata template generated at \"" + fullpath + "\"" << std::endl;
    }

    // Execute the full OAuth authentication flow
    static void authenticate() {
        try {
            auto settings = Settings::createInstance();

            auto clientId = settings->get("client_id");
            auto clientSecret = settings->get("client_secret");
            auto authEndpoint = settings->get("auth_endpoint");
            auto tokenEndpoint = settings->get("token_endpoint");
            auto redirectUri = settings->get("redirect_uris");

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
            settings->set("accessToken", accessToken);
            std::cout << "Access Token: " << accessToken << std::endl;
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
};

#endif // OAUTHCLIENT_H