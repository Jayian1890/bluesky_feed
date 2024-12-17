//
// Created by jayian on 11/28/24.
// Copyright (c) 2024 Interlaced Pixel. All rights reserved.
//

#ifndef URL_ENCODER_H
#define URL_ENCODER_H



#pragma once

#include <string>
#include <stdexcept>

class URLEncoder {
public:
    // Encodes a full URL while preserving the protocol and domain
    static std::string encodeURL(std::string_view url);

    // Encodes a specific URL component (e.g., query parameters)
    static std::string encodeComponent(std::string_view value);

    // Decodes a URL or component
    static std::string decode(const std::string& value);
};



#endif //URL_ENCODER_H
