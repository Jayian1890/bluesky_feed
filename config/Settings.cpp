//
// Created by jayian on 11/26/24.
// Copyright (c) 2024 Interlaced Pixel. All rights reserved.
//

#include <stdexcept>
#include <vector>
#include <fstream>
#include <iostream>
#include "Settings.h"

Settings::Settings(const std::string& filename) : filePath(filename) {
    verifyAndUpdateFileStream(std::ios::in | std::ios::out | std::ios::app);

    if (!fileStream.is_open()) {
        if (filePath == "settings.config") {
            createDefaultSettings();
        } else {
            throw std::runtime_error("Failed to open settings file: " + filePath);
        }
    }

    loadSettings();
}

Settings::~Settings() {
    destroy();
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

void Settings::createDefaultSettings() {
    json.set("client_id", R"(https://bsky.interlacedpixel.com/client-metadata.json)");
    json.set("client_secret", "");
    json.set("accessToken", "");
    json.set("public_api", "https://public.api.bsky.app");
    json.set("auth_endpoint", "https://bsky.social/oauth/authorize");
    json.set("token_endpoint", "https://bsky.social/oauth/token");
    json.set("redirect_uri", "");
    saveSettings();
}

bool Settings::hasKey(const std::string& key) const {
    std::vector<std::string> keys = json.getKeys();
    return std::find(keys.begin(), keys.end(), key) != keys.end();
}

std::string Settings::get(const std::string& key) {
    loadSettings();
    return json.get(key);
}

void Settings::set(const std::string& key, const std::string& value) {
    json.set(key, value);
    saveSettings();
}

void Settings::destroy() {
    json.clear();
    if (fileStream.is_open()) {
        fileStream.close();
    }
}

void Settings::resetFileStream(const std::ios::openmode& mode) {
    if (fileStream.is_open()) {
        fileStream.close();
        fileStream.seekg(0, std::ios::beg);
    }

    if (!fileStream.is_open()) {
        throw std::runtime_error("Failed to reopen file: " + filePath);
    }

    verifyAndUpdateFileStream(mode);
}

void Settings::reset(const std::ios::openmode fileMode) {
    destroy();
    resetFileStream(fileMode);
}

void Settings::loadSettings() {
    verifyAndUpdateFileStream(std::ios::in);

    fileStream.clear();
    fileStream.seekg(0, std::ios::beg);

    if (fileStream.peek() != std::ifstream::traits_type::eof()) {
        const std::string content((std::istreambuf_iterator(fileStream)), std::istreambuf_iterator<char>());
        if (!content.empty()) {
            json.parse(content);
        }
    } else {
        std::cerr << "Warning: Settings file is empty." << std::endl;
    }
}


void Settings::saveSettings() {
    verifyAndUpdateFileStream(std::ios::out | std::ios::trunc);
    fileStream << json.generate();
    fileStream.flush();
}

void Settings::verifyAndUpdateFileStream(const std::ios::openmode requiredMode) {
    if (!fileStream.is_open()) {
        fileStream.open(filePath, requiredMode);
        if (!fileStream.is_open()) {
            throw std::runtime_error("Failed to open settings file with required mode: " + filePath);
        }
    } else {
        if (const std::ios::openmode currentMode = fileStream.flags(); (currentMode & requiredMode) != requiredMode) {
            fileStream.close();
            fileStream.open(filePath, requiredMode);
            if (!fileStream.is_open()) {
                throw std::runtime_error("Failed to reopen settings file with required mode: " + filePath);
            }
        }
    }
}


Settings& Settings::getInstance(const std::string& filename) {
    static Settings instance(filename);
    return instance;
}