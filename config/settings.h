//
// Created by jayian on 11/26/24.
// Copyright (c) 2024 Interlaced Pixel. All rights reserved.
//

#ifndef SETTINGS_H
#define SETTINGS_H

#pragma once

#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "../tools/json.h"

class Settings {
    std::string filePath = "settings.config";
    JSON json{};

    // Private constructor
    Settings();

    // Helper to get absolute path
    static std::string getAbsolutePath(const std::string& relativePath);

    // Create default settings if the file does not exist or is empty
    void createDefaultSettings() const;

    // Save the current settings to file
    void saveSettings() const;

public:
    // Singleton pattern for global access to settings
    static Settings& getInstance();

    // Get a value by key
    [[nodiscard]] std::string get(const std::string& key) const;

    // Check if a key exists
    [[nodiscard]] bool hasKey(const std::string& key) const;

    // Set a value by key
    void set(const std::string& key, const std::string& value);
};

#endif //SETTINGS_H
