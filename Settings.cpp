//
// Created by Jared T on 11/26/24.
//

#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "json.cpp"

class Settings {
private:
    std::string filePath = "settings.config";
    JSON json;

    Settings() {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            createDefaultSettings();
            file.open(filePath);
        }
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        if (content.empty()) {
            createDefaultSettings();
            file.open(filePath);
            content = R"({
"public_api": "https://public.api.bsky.app"
})";
        }
        file.close();
        json.parse(content);
    }

    static std::string getAbsolutePath(const std::string& relativePath) {
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

    void createDefaultSettings() const {
        std::ofstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to create settings file: " + filePath);
        }
        file << R"({
            "public_api": "https://public.api.bsky.app"
        })";
        file.close();
    }

public:
    std::string get(const std::string& key) const {
        return json.get(key);
    }

    bool hasKey(const std::string& key) const {
        std::vector<std::string> keys = json.getKeys();
        return std::find(keys.begin(), keys.end(), key) != keys.end();
    }

    void set(const std::string& key, const std::string& value) {
        json.set(key, value);
        saveSettings();
    }

    void saveSettings() const {
        std::ofstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to write settings to file: " + filePath);
        }
        file << json.generate();
        file.close();
    }

    static Settings& getInstance() {
        static Settings instance;
        return instance;
    }
};