//
// Created by jayian on 11/26/24.
// Copyright (c) 2024 Interlaced Pixel. All rights reserved.
//

#include "../actor/getProfile.cpp"
#include "../network/OAuthClient.hpp"
#include "CommandHandler.h"

// Execute a command
void CommandHandler::executeCommand(const std::string& command, const std::vector<std::string>& args) {
    if (command == "getprofile") {
        if (args.size() != 1) {
            std::cerr << "Error: getprofile requires exactly one argument." << std::endl;
        } else {
            std::cout << GetProfile(args[0]).getData() << std::endl;
        }
    } else if (command == "metadata") {
        OAuthClient::generateClientMetadata();
    } else if (command == "oauth") {
        OAuthClient client("your_client_id", "your_client_secret",
                   "https://example.com/auth", "https://example.com/token",
                   "http://localhost:8080/callback");

        client.authenticate();
        std::cout << "Access Token: " << client.getAccessToken() << std::endl;
    } else if (command == "help") {
        printHelp();
    } else {
        std::cout << "Unknown command: " << command << std::endl;
    }
}

// Print help message
void CommandHandler::printHelp() {
    std::cout << "Available commands:" << std::endl;
    std::cout << "  getprofile <name>     - Returns details for the specified profile" << std::endl;
    std::cout << "  oauth                 - Authenticates the OAuth client with Bluesky API" << std::endl;
    std::cout << "  metadata              - Assists with creating a client-metadata.json file." << std::endl;
    std::cout << "  help                  - Shows this help message" << std::endl;
    std::cout << "  exit                  - Exit the program" << std::endl;
}