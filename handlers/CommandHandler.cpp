//
// Created by jayian on 11/26/24.
// Copyright (c) 2024 Interlaced Pixel. All rights reserved.
//

#include "../actor/getProfile.cpp"
#include "CommandHandler.h"

// Execute a command
void CommandHandler::executeCommand(const std::string& command, const std::vector<std::string>& args) {
    if (command == "parse") {
        if (args.empty()) {
            std::cout << "Error: parse requires a JSON string as argument." << std::endl;
            return;
        }
        JSON().parse(args[0]);
    } else if (command == "getprofile") {
        if (args.empty()) {
            std::cout << "Error: getprofile requires a profile name as argument." << std::endl;
            return;
        }
        const getProfile profile(args[0]);
        std::cout << profile.generate() << std::endl;
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
    std::cout << "  help                  - Shows this help message" << std::endl;
    std::cout << "  exit                  - Exit the program" << std::endl;
}