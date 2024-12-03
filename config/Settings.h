//
// Created by jayian on 11/26/24.
// Copyright (c) 2024 Interlaced Pixel. All rights reserved.
//

#ifndef SETTINGS_H
#define SETTINGS_H

#pragma once

#include <string>
#include <unordered_map>
#include "../nlohmann/json.hpp"

class Settings {
    static constexpr auto DEFAULT_SETTINGS_PATH = "settings.json";

    nlohmann::json json; // JSON object for managing settings
    const std::string filePath; // Path to the settings file
    const std::unordered_map<std::string, std::string> defaultSettings;

    void loadSettings();
    void saveSettings();
    void loadAndEnsureDefaults(const std::unordered_map<std::string, std::string>& defaults);

    static bool isFileEmpty(const std::string& path);
    static std::unordered_map<std::string, std::string> getDefaultSettings();

public:
    explicit Settings(const std::string& filename, const std::unordered_map<std::string, std::string>& defaults = {});
    ~Settings() = default;

    std::string get(const std::string& key);
    bool hasKey(const std::string& key) const;
    void set(const std::string& key, const std::string& value);

    static std::string getAbsolutePath(const std::string& relativePath);
    static std::string promptAndSet(const std::string& description, const std::string& key, Settings& settings);

    static std::unique_ptr<Settings> createInstance(const std::string& filename = DEFAULT_SETTINGS_PATH,
        const std::unordered_map<std::string, std::string>& defaults = {});
};

#endif // SETTINGS_H