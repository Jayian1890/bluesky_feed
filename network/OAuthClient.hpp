#ifndef OAUTHCLIENT_H
#define OAUTHCLIENT_H

#pragma once

#include <string>
#include <unordered_map>
#include <thread>
#include <iostream>
#include "RedirectUriHandler.hpp"
#include "../nlohmann/json.hpp"
#include "../config/Settings.h"
#include "../tools/url_encoder.h"
#include "../tools/logging.hpp"
#include "../cpp-httplib/httplib.h" // Include httplib for HTTP requests


class OAuthException final : public std::exception {
    std::string message;

public:
    explicit OAuthException(std::string msg) : message(std::move(msg)) {}

    [[nodiscard]] const char* what() const noexcept override {
        return message.c_str();
    }
};

class OAuthClient {
    std::string clientId;
    std::string clientSecret;
    std::string authEndpoint;
    std::string tokenEndpoint;
    std::string redirectUri;
    std::string accessToken;
    RedirectUriHandler redirectHandler;

    // Utility function to percent-encode strings for URLs
    static std::string urlEncode(const std::string &value) {
        std::ostringstream encoded;
        encoded.fill('0');
        encoded << std::hex;

        for (const unsigned char c : value) {
            // Keep alphanumeric and some symbols unescaped
            if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
                encoded << c;
            } else {
                // Escape the character
                encoded << '%' << std::setw(2) << static_cast<int>(c);
            }
        }
        return encoded.str();
    }

    static std::string urlencode(const std::unordered_map<std::string, std::string>& params) {
        std::string encoded;
        for (const auto& [key, value] : params) {
            if (!encoded.empty()) {
                encoded += "&";
            }
            encoded += urlEncode(key) + "=" + urlEncode(value);
        }
        return encoded;
    }

    static std::string postRequest(const std::string_view url, const std::unordered_map<std::string, std::string>& postData) {
        // Parse URL to get the host and endpoint
        const auto lastSlashPos = url.find_last_of('/');
        if (lastSlashPos == std::string_view::npos) {
            throw std::invalid_argument("Invalid URL format: " + std::string(url));
        }

        const auto host = std::string(url.substr(0, lastSlashPos));
        const auto endpoint = std::string(url.substr(lastSlashPos));

        httplib::Client cli(host);
        auto response = cli.Post(endpoint, urlencode(postData), "application/x-www-form-urlencoded");

        if (!response || response->status != 200) {
            throw HTTPException("HTTP POST failed: " +
                                (response ? std::to_string(response->status) : "No response"));
        }

        return response->body;
    }

public:
    OAuthClient(std::string clientId, std::string clientSecret, std::string authEndpoint,
            std::string tokenEndpoint, const std::string & redirectUri)
    : clientId(std::move(clientId)),
      clientSecret(std::move(clientSecret)),
      authEndpoint(std::move(authEndpoint)),
      tokenEndpoint(std::move(tokenEndpoint)),
      redirectUri(redirectUri),
      redirectHandler(redirectUri)
    {}

    [[nodiscard]] std::string getAuthUrl(const std::string& scope, const std::string& state) const {
        return authEndpoint + "?response_type=code&client_id=" + clientId +
               "&redirect_uri=" + redirectUri + "&scope=" + URLEncoder::encodeComponent(scope) +
               "&state=" + state;
    }

    void fetchAccessToken(const std::string& authCode) {
        const std::unordered_map<std::string, std::string> postData = {
            {"grant_type", "authorization_code"},
            {"code", authCode},
            {"redirect_uri", redirectUri},
            {"client_id", clientId},
            {"client_secret", clientSecret}
        };

        const auto response = postRequest(tokenEndpoint, postData);
        accessToken = nlohmann::json::parse(response).at("access_token").get<std::string>();

        if (accessToken.empty()) throw OAuthException("Failed to fetch access token");
    }

    [[nodiscard]] const std::string& getAccessToken() const { return accessToken; }

    // Updated authenticate method
    void authenticate(const std::string& scope = "read_profile write_profile", const std::string& state = "random_state") {
        try {
            // Start the redirect server in a separate thread
            std::thread serverThread([this] { redirectHandler.startServer(); });

            // Generate and display the authorization URL
            const std::string authUrl = getAuthUrl(scope, state);
            Logging::info("Visit this URL to authorize: " + authUrl);

            // Wait for the user to authorize and enter the redirect code
            serverThread.join(); // Blocks until code is received
            const std::string authCode = redirectHandler.getAuthorizationCode();

            // Exchange the authorization code for an access token
            fetchAccessToken(authCode);

            Logging::info("Access Token: " + accessToken);
        } catch (const OAuthException& e) {
            Logging::error("Error during OAuth process: " + std::string(e.what()));
        }
    }

    static std::unordered_map<std::string, std::string> generateOAuthClientInfo() {
        return {
                    {"client_id", "https://example.com/client-metadata.json"},
                    {"client_name", "Example App"},
                    {"client_uri", "https://example.com"},
                    {"logo_uri", "https://example.com/logo.png"},
                    {"tos_uri", "https://example.com/tos"},
                    {"policy_uri", "https://example.com/policy"},
                    {"redirect_uris", "[\"https://example.com/callback\"]"},
                    {"scope", "atproto"},
                    {"grant_types", R"(["authorization_code", "refresh_token"])"},
                    {"response_types", "[\"code\"]"},
                    {"token_endpoint_auth_method", "none"},
                    {"application_type", "web"},
                    {"dpop_bound_access_tokens", "true"}
        };
    }

    static void generateClientMetadata() {
        const auto clientInfo = generateOAuthClientInfo();
        const auto metadataSettings = Settings::createInstance("client-metadata.json", clientInfo);

        for (const auto &[key, defaultValue]: clientInfo) {
            if (!metadataSettings->hasKey(key)) {
                metadataSettings->set(key, defaultValue);
            }
        }

        const auto path = Settings::getAbsolutePath("client-metadata.json");
        Logging::info("[OAuthClient] Client metadata generated at: '" + path + "'");
    }
};

#endif // OAUTHCLIENT_H