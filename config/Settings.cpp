//
// Created by jayian on 11/26/24.
// Copyright (c) 2024 Interlaced Pixel. All rights reserved.
//

#include <fstream>
#include <stdexcept>
#include <vector>
#include "Settings.h"

Settings::Settings() {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        createDefaultSettings();
        file.open(filePath);
    }
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    if (content.empty()) {
        createDefaultSettings();
    }
    file.close();
    json.parse(content);
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

void Settings::loadSettings() {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open settings file: " + filePath);
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    if (content.empty()) {
        createDefaultSettings();
        file.close();
        file.open(filePath);
        content = json.generate();
    }
    file.close();
    json.parse(content);
}

void Settings::createDefaultSettings() {
    set("client_id", "");
    set("client_secret", "");
    set("accessToken", "");
    set("public_api", "https://public.api.bsky.app");
    set("auth_endpoint", "https://api.bsky.app/oauth/authorize");
    set("token_endpoint", "https://api.bsky.app/oauth/token");
    set("redirect_uri", "");
}

std::string Settings::get(const std::string& key) {
    loadSettings();
    return json.get(key);
}

bool Settings::hasKey(const std::string& key) const {
    std::vector<std::string> keys = json.getKeys();
    return std::find(keys.begin(), keys.end(), key) != keys.end();
}

void Settings::set(const std::string& key, const std::string& value) {
    json.set(key, value);
    saveSettings();
}

void Settings::saveSettings() const {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to write settings to file: " + filePath);
    }
    file << json.generate();
    file.close();
}

Settings& Settings::getInstance() {
    static Settings instance;
    return instance;
}
