//
// Created by jayian on 11/26/24.
// Copyright (c) 2024 Interlaced Pixel. All rights reserved.
//
/*
#pragma once

#include <iostream>
#include "../config/Settings.h"
#include "../network/https_client.h"

class getProfile : public nlohmann::json {
    std::string _host = Settings::createInstance()->get("public_api");
    std::string _endpoint = "/xrpc/app.bsky.actor.getProfile";
    std::string _bearerToken;
    std::string _actor;

public:
    explicit getProfile(std::string actor)
        : _actor(std::move(actor)) {
        this->fetch();
    }

    void setActor(std::string actor) {
        this->_actor = std::move(actor);
    }

    void fetch() {
        HTTPSClient client;
        client.setHost(_host);
        client.setEndpoint(_endpoint);
        client.setBearerToken(_bearerToken);

        client.addQueryParam("actor", _actor);

        try {
            const nlohmann::json response = client.get();
            for (const auto& key : response.getKeys()) {
                set(key, response.get(key));
            }
        } catch (const std::exception& e) {
            std::cerr << "Error fetching preferences: " << e.what() << "\n";
        }
    }
};*/