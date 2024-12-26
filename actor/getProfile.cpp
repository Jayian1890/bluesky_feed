//
// Created by jayian on 11/26/24.
// Copyright (c) 2024 Interlaced Pixel. All rights reserved.
//

#include "../config/Settings.h"
#include "../network/https_client.h"
#include "../nlohmann/json.hpp"
#include "../tools/logging.hpp"

class SettingsLoadException final : public std::runtime_error {
public:
    explicit SettingsLoadException(const std::string& message)
        : std::runtime_error("SettingsLoadException: " + message) {}
};

class FetchingProfileException final : public std::runtime_error {
public:
    explicit FetchingProfileException(const std::string& message)
        : std::runtime_error("FetchingProfileException: " + message) {}
};

class GetProfile {
    std::string _host;
    std::string _endpoint = "/xrpc/app.bsky.actor.getProfile";
    std::string _bearerToken;
    std::string _actor;
    nlohmann::json _data;

public:
    explicit GetProfile(std::string actor)
    : _actor(std::move(actor)) {
        initializeSettings();
        fetch();
    }

    void setActor(std::string actor) {
        _actor = std::move(actor);
    }

    [[nodiscard]] const nlohmann::json& getData() const {
        return _data;
    }

    void fetch() {
        HTTPSClient client;
        client.setHost(_host);
        client.setEndpoint(_endpoint);
        client.setBearerToken(_bearerToken);
        client.addQueryParam("actor", _actor);

        try {
            _data = client.get(); // Fetch the data
        } catch (const FetchingProfileException& e) {
            Logging::error("Error fetching profile: ");
            Logging::error(e.what());
        }
    }

private:
    void initializeSettings() {
        try {
            const auto settings = Settings::createInstance();
            _host = settings->get<std::string>("public_api");
            _bearerToken = settings->get<std::string>("accessToken");
        } catch (const std::exception& e) {
            throw SettingsLoadException("Failed to load settings: " + std::string(e.what()));
        }
    }
};