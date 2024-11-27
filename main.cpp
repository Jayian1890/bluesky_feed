//
// Created by jayian on 11/26/24.
// Copyright (c) 2024 Interlaced Pixel. All rights reserved.
//

#include <iostream>
#include <sstream>
#include "handlers/commandHandler.cpp"

// Helper function to split input into command and arguments
std::pair<std::string, std::vector<std::string>> parseInput(const std::string& input) {
    std::istringstream stream(input);
    std::string command;
    std::vector<std::string> args;
    std::string arg;

    stream >> command;
    while (stream >> arg) {
        args.push_back(arg);
    }

    return {command, args};
}

int main() {
    std::string input;

    std::cout << "Welcome to IP-Feed Console!" << std::endl;
    std::cout << "Type 'help' for a list of commands." << std::endl;

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);

        auto [command, args] = parseInput(input);

        if (command == "exit") {
            std::cout << "Goodbye!" << std::endl;
            break;
        }

        commandHandler::executeCommand(command, args);
    }

    return 0;
}