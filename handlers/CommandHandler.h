//
// Created by jayian on 11/26/24.
// Copyright (c) 2024 Interlaced Pixel. All rights reserved.
//

#ifndef COMMANDHANDLER_H
#define COMMANDHANDLER_H

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "../config/Settings.h"

class CommandHandler {
    Settings& settings = Settings::getInstance(); // Reference to Settings instance

public:
    // Execute a command with arguments
    static void executeCommand(const std::string& command, const std::vector<std::string>& args);

    // Print help message for available commands
    static void printHelp();
};


#endif //COMMANDHANDLER_H
