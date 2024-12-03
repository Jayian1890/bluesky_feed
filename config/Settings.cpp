//
// Created by jayian on 11/26/24.
// Copyright (c) 2024 Interlaced Pixel. All rights reserved.
//

#include "Settings.h"
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <fstream>

// Constructor
Settings::Settings(const std::string& filename, const std::unordered_map<std::string, std::string>& defaults)
    : filePath(filename), defaultSettings(defaults.empty() ? getDefaultSettings() : defaults) {
    loadAndEnsureDefaults(defaultSettings);
}

// Check if a file is empty
bool Settings::isFileEmpty(const std::string& path) {
    std::ifstream file(path, std::ios::ate | std::ios::binary);
    return file.tellg() == 0;
}

// Load settings from the file or create it if missing
void Settings::loadSettings() {
    std::ifstream inFile(filePath);
    if (!inFile) {
        // File doesn't exist, create it
        std::ofstream outFile(filePath, std::ios::out);
        if (!outFile) {
            throw std::runtime_error("Failed to create settings file: " + filePath);
        }
        json = defaultSettings; // Initialize with defaults
        outFile << json.dump(4); // Save defaults to the file
        outFile.close();
        return;
    }

    if (inFile.peek() != std::ifstream::traits_type::eof()) {
        try {
            inFile >> json; // Parse JSON content
        } catch (const std::exception& e) {
            throw std::runtime_error("Invalid JSON format in settings file: " + std::string(e.what()));
        }
    }
    inFile.close();
}

// Save settings to the file
void Settings::saveSettings() {
    std::ofstream outFile(filePath, std::ios::out | std::ios::trunc);
    if (!outFile) {
        throw std::runtime_error("Failed to open settings file for writing: " + filePath);
    }
    outFile << json.dump(4); // Pretty print JSON with 4 spaces
    outFile.close();
}

// Ensure defaults are loaded or added if necessary
void Settings::loadAndEnsureDefaults(const std::unordered_map<std::string, std::string>& defaults) {
    loadSettings();

    for (const auto& [key, defaultValue] : defaults) {
        if (!json.contains(key) || json[key].is_null() || json[key].get<std::string>().empty()) {
            json[key] = defaultValue; // Add missing defaults
        }
    }

    saveSettings(); // Save updated settings back to the file
}

std::string Settings::get(const std::string& key) {
    loadSettings();
    if (!json.contains(key)) {
        throw std::runtime_error("Key not found: " + key);
    }
    return json[key].get<std::string>();
}

bool Settings::hasKey(const std::string& key) const {
    return json.contains(key);
}

void Settings::set(const std::string& key, const std::string& value) {
    json[key] = value;
    saveSettings();
}

// Utility function to check if a file is empty
std::unordered_map<std::string, std::string> Settings::getDefaultSettings() {
    return {
        {"client_id", R"(https://bsky.interlacedpixel.com/client-metadata.json)"},
        {"client_secret", "REPLACE_WITH_TOKEN"},
        {"accessToken", "REPLACE_WITH_TOKEN"},
        {"public_api", "https://public.api.bsky.app"},
        {"auth_endpoint", "https://bsky.social/oauth/authorize"},
        {"token_endpoint", "https://bsky.social/oauth/token"},
        {"redirect_uri", "https://bsky.interlacedpixel.com/redirect"},
        {"feed_name", "Interlaced Pixel Test"}
    };
}

std::string Settings::getAbsolutePath(const std::string& relativePath) {
#ifdef _WIN32
    char fullPath[MAX_PATH];
    if (GetFullPathName(relativePath.c_str(), MAX_PATH, fullPath, nullptr) == 0) {
        throw std::runtime_error("Failed to resolve full path for: " + relativePath);
    }
    return std::string(fullPath);
#else
    char fullPath[PATH_MAX];
    if (realpath(relativePath.c_str(), fullPath) == nullptr) {
        throw std::runtime_error("Failed to resolve full path for: " + relativePath);
    }
    return {fullPath};
#endif
}

std::string Settings::promptAndSet(const std::string& description, const std::string& key, Settings& settings) {
    std::string value;
    std::cout << description << std::endl;
    std::cout << "Enter " + key + ": ";
    std::getline(std::cin, value);
    settings.set(key, value);
    return value;
}

std::unique_ptr<Settings> Settings::createInstance(const std::string& filename, const std::unordered_map<std::string, std::string>& defaults) {
    return std::make_unique<Settings>(filename, defaults);
}