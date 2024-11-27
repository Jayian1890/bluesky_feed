//
// Created by jayian on 11/26/24.
// Copyright (c) 2024 Interlaced Pixel. All rights reserved.
//

#ifndef JSON_H
#define JSON_H

#pragma once

#include <string>
#include <map>
#include <vector>

class JSON {
    std::map<std::string, std::string> data;
    std::vector<std::string> keys;

public:
    void parse(const std::string& jsonString);

    // Get a value by key
    [[nodiscard]] std::string get(const std::string& key) const;

    // Set a key-value pair
    void set(const std::string& key, const std::string& value);

    // Get the list of keys
    [[nodiscard]] std::vector<std::string> getKeys() const;

    // Generate a JSON string from the data
    [[nodiscard]] std::string generate() const;
};


#endif //JSON_H
