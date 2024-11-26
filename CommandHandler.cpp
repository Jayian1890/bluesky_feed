//
// Created by Jared T on 11/26/24.
//

#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include "actor/getProfile.cpp"
#include "Settings.cpp"


class CommandHandler {
public:
    static void executeCommand(const std::string& command, const std::vector<std::string>& args) {
        if (command == "parse") {
            if (args.empty()) {
                std::cout << "Error: parse requires a JSON string as argument." << std::endl;
                return;
            }
            JSON().parse(args[0]);
        } else if (command == "getprofile") {
            const getProfile profile(args[0]);
            std::cout << profile.generate() << std::endl;
        } else if (command == "help") {
            printHelp();
        } else {
            std::cout << "Unknown command: " << command << std::endl;
        }
    }

    static void printHelp() {
        std::cout << "Available commands:" << std::endl;
        //std::cout << "  parse <json_string>  - Parses a JSON string" << std::endl;
        std::cout << "  getprofile           - Returns details for the Profile" << std::endl;
        std::cout << "  help                 - Shows this help message" << std::endl;
        std::cout << "  exit                 - Exit the program" << std::endl;
    }
};