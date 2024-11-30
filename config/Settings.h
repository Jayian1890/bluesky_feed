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

#pragma once

#include <fstream>
#include "../tools/json.h"

class Settings {
    JSON json;
    const std::string& filePath;
    std::fstream fileStream;

    explicit Settings(const std::string& filename);
    void destroy();
    void resetFileStream(const std::ios::openmode& mode);
    void reset(std::ios::openmode fileMode = std::ios::in | std::ios::out | std::ios::app);
    void loadSettings();
    void createDefaultSettings();
    void verifyAndUpdateFileStream(std::ios::openmode requiredMode);

public:
    static Settings& getInstance(const std::string& filename = "settings.config");
    ~Settings();

    std::string get(const std::string& key);
    bool hasKey(const std::string& key) const;
    void set(const std::string& key, const std::string& value);
    void saveSettings();

    static std::string getAbsolutePath(const std::string& relativePath);
    static std::string promptAndSet(const std::string& description, const std::string& key, Settings& settings);
};


#endif //SETTINGS_H
