//
// Created by jayian on 11/28/24.
// Copyright (c) 2024 Interlaced Pixel. All rights reserved.
//

#include "url_encoder.h"
#include <iomanip>
#include <sstream>

// Helper function to determine if a character should be encoded
static bool isUnreservedOrPath(const unsigned char c) {
    return std::isalnum(c) || c == '-' || c == '.' || c == '_' || c == '~' || c == '/';
}

// Encodes a URL component (e.g., query parameters)
std::string URLEncoder::encodeComponent(const std::string_view value) {
    std::ostringstream encoded;
    encoded << std::hex << std::uppercase;

    for (const unsigned char c : value) { // Explicitly interpret as unsigned char
        if (isUnreservedOrPath(c)) {
            encoded << static_cast<char>(c); // Write as character
        } else {
            encoded << '%' << std::setw(2) << std::setfill('0') << static_cast<int>(c);
        }
    }

    return encoded.str();
}

// Decodes a URL or component
std::string URLEncoder::decode(const std::string& value) {
    std::ostringstream decoded;
    size_t i = 0;

    while (i < value.length()) {
        if (value[i] == '%') {
            if (i + 2 >= value.length()) {
                throw std::invalid_argument("Invalid percent-encoding in URL");
            }

            // Decode the next two hex digits
            std::string hex = value.substr(i + 1, 2);
            const auto decodedChar = static_cast<char>(std::stoi(hex, nullptr, 16));
            decoded << decodedChar;
            i += 3; // Skip the % and the two hex digits
        } else if (value[i] == '+') {
            decoded << ' '; // Convert '+' to space
            ++i;
        } else {
            decoded << value[i]; // Copy other characters as is
            ++i;
        }
    }

    return decoded.str();
}

// Encodes a full URL while preserving the protocol, domain, and path
std::string URLEncoder::encodeURL(std::string_view url) {
    const size_t protocolEnd = url.find("://");
    if (protocolEnd == std::string_view::npos) {
        throw std::invalid_argument("Invalid URL: Missing protocol");
    }

    // Find where the domain ends (assume path/query/fragments start after '/')
    const size_t domainEnd = url.find('/', protocolEnd + 3);

    // Extract parts of the URL
    const std::string_view protocolAndDomain = url.substr(0, domainEnd);
    const std::string_view pathAndQuery = domainEnd != std::string_view::npos ? url.substr(domainEnd) : "";

    // Encode the path and query parts
    const std::string encodedPathAndQuery = encodeComponent(pathAndQuery);

    // Combine the parts back together
    return std::string(protocolAndDomain) + encodedPathAndQuery;
}