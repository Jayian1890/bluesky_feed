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
#include "../tools/json.h"
#include "../config/Settings.h"
#include "../tools/url_encoder.h"

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
               "&redirect_uri=" + redirectUri + "&scope=" + URLEncoder::encodeComponent(scope) + "&state=" + state;
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

    static void createClientMetadata() {
        auto& metadataSettings = Settings::getInstance("client-metadata.json");

        std::cout << "[OAuthClient] Creating client-metadata..." << std::endl;

        struct MetadataInput {
            std::string description;
            std::string key;
        };

        const std::vector<MetadataInput> inputs = {
            {"A client ID must be a URL pointing to a JSON file which contains your client metadata.", "client_id"},
            {"The name of your client. Will be displayed to the user during the authentication process.", "client_name"},
            {"The URL of your client. Whether or not this value is actually displayed/used is up to the Authorization Server.", "client_uri"},
            {"The URL of your client's logo. Should be displayed to the user during the authentication process. Whether your logo is actually displayed during the authentication process or not is up to the Authorization Server.", "logo_uri"},
            {"The URL of your client's terms of service. Will be displayed to the user during the authentication process.", "tos_uri"},
            {"The URL of your client's privacy policy. Will be displayed to the user during the authentication process.", "policy_uri"},
            {"An array of URLs that will be used as the redirect URIs for the OAuth flow. This should typically contain a single URL that points to a page on your SPA that will handle the OAuth response. This URL must be HTTPS.", "redirect_uris"}
        };

        for (const auto&[description, key] : inputs) {
            if (metadataSettings.hasKey(key)) {
                std::string currentValue = metadataSettings.get(key);
                std::cout << "Current value for " << key << ": " << currentValue << std::endl;
                std::cout << "Would you like to update it? (y/n): ";
                std::string response;
                std::getline(std::cin, response);

                if (response == "y" || response == "Y") {
                    const auto newValue = Settings::promptAndSet(description, key, metadataSettings);
                    if (key == "client_id") {
                        Settings::getInstance().set("client_id", newValue);
                    }
                } else {
                    std::cout << "Keeping the current value for " << key << "." << std::endl;
                }
            } else {
                const auto newValue = Settings::promptAndSet(description, key, metadataSettings);
                if (key == "client_id") {
                    Settings::getInstance().set("client_id", newValue);
                }
            }
        }
    }


    static void authenticate() {
        try {
            auto& settings = Settings::getInstance();

            auto clientId = settings.get("client_id");
            auto clientSecret = settings.get("client_secret");
            auto authEndpoint = settings.get("auth_endpoint");
            auto tokenEndpoint = settings.get("token_endpoint");
            auto redirectUri = settings.get("redirect_uri");

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