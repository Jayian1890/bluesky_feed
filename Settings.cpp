//
// Created by Jared T on 11/26/24.
//

#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include "json.cpp"

class Settings {
private:
    JSON json;

public:
    explicit Settings(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open settings file: " + filePath);
        }

        const std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();

        json.parse(content);
    }

    // Get a value by key
    std::string get(const std::string& key) const {
        return json.get(key);
    }

    // Check if a key exists
    bool hasKey(const std::string& key) const {
        std::vector<std::string> keys = json.getKeys();
        return std::find(keys.begin(), keys.end(), key) != keys.end();
    }
};