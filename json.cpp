//
// Created by Jared T on 11/26/24.
//

#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>

class JSON {
private:
    std::map<std::string, std::string> data;
    std::vector<std::string> keys;

public:
    void parse(const std::string& jsonString) {
        size_t pos = 0;
        size_t keyStart = 0;
        size_t valueStart = 0;
        bool inQuotes = false;
        bool parsingKey = true;
        std::string key;

        while (pos < jsonString.length()) {
            char currentChar = jsonString[pos];

            if (isspace(currentChar)) {
                ++pos;
                continue;
            }

            if (currentChar == '"') {
                if (inQuotes) {
                    if (parsingKey) {
                        key = jsonString.substr(keyStart, pos - keyStart);
                    } else {
                        std::string value = jsonString.substr(valueStart, pos - valueStart);
                        data[key] = value;
                        keys.push_back(key);
                    }
                    inQuotes = false;
                    parsingKey = !parsingKey;
                } else {
                    inQuotes = true;
                    if (parsingKey) {
                        keyStart = pos + 1;
                    } else {
                        valueStart = pos + 1;
                    }
                }
            }

            if (currentChar == ',' && !inQuotes) {
                parsingKey = true;
            }

            if (currentChar == '}' || currentChar == ']') {
                break;
            }

            ++pos;
        }
    }

    // Get a value by key
    std::string get(const std::string& key) const {
        if (data.find(key) != data.end()) {
            return data.at(key);
        }
        return "";
    }

    // Set a key-value pair
    void set(const std::string& key, const std::string& value) {
        data[key] = value;
        keys.push_back(key);
    }

    // Get the list of keys
    std::vector<std::string> getKeys() const {
        return keys;
    }

    // Generate a JSON string from the data
    std::string generate() const {
        std::stringstream ss;
        ss << "{";

        for (size_t i = 0; i < keys.size(); ++i) {
            const std::string& key = keys[i];
            const std::string& value = data.at(key);

            ss << "\"" << key << "\": \"" << value << "\"";
            if (i != keys.size() - 1) {
                ss << ",";
            }
        }

        ss << "}";
        return ss.str();
    }
};