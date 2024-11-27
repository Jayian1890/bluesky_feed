//
// Created by jayian on 11/26/24.
// Copyright (c) 2024 Interlaced Pixel. All rights reserved.
//

#include "JSON.h"
#include <sstream>

void JSON::parse(const std::string& jsonString) {
    size_t pos = 0;
    size_t keyStart = 0;
    size_t valueStart = 0;
    bool inQuotes = false;
    bool parsingKey = true;
    std::string key;

    while (pos < jsonString.length()) {
        const char currentChar = jsonString[pos];

        if (isspace(currentChar)) {
            ++pos;
            continue;
        }

        if (currentChar == '"') {
            if (inQuotes) {
                if (parsingKey) {
                    key = jsonString.substr(keyStart, pos - keyStart);
                } else {
                    const std::string value = jsonString.substr(valueStart, pos - valueStart);
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

std::string JSON::get(const std::string& key) const {
    if (data.find(key) != data.end()) {
        return data.at(key);
    }
    return "";
}

void JSON::set(const std::string& key, const std::string& value) {
    if (data.find(key) != data.end()) {
        data[key] = value;
    } else {
        data[key] = value;
        keys.push_back(key);
    }
}

std::vector<std::string> JSON::getKeys() const {
    return keys;
}

std::string JSON::generate() const {
    std::stringstream ss;
    int indentLevel = 0;

    auto addIndent = [&ss, &indentLevel]() {
        for (int i = 0; i < indentLevel; ++i) {
            ss << "    "; // 4 spaces per indent
        }
    };

    ss << "{\n";
    ++indentLevel;

    for (size_t i = 0; i < keys.size(); ++i) {
        const std::string& key = keys[i];
        const std::string& value = data.at(key);

        addIndent();
        ss << "\"" << key << "\": \"" << value << "\"";

        if (i != keys.size() - 1) {
            ss << ",";
        }
        ss << "\n";
    }

    --indentLevel;
    addIndent();
    ss << "}";

    return ss.str();
}
