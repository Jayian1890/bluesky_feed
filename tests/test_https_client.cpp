//
// Created by jayian on 12/25/24.
// Copyright (c) 2024 Interlaced Pixel. All rights reserved.
//

#include <gtest/gtest.h>
#include "../network/https_client.h"
#include "../tools/logging.hpp"

// Mock or real endpoint for testing
const std::string TEST_HOST = "api.github.com";
const std::string TEST_ENDPOINT = "/";

TEST(HTTPSClientTest, SuccessfulGETRequest) {
    HTTPSClient client;
    client.setHost(TEST_HOST);
    client.setEndpoint(TEST_ENDPOINT);
    client.addQueryParam("test", "value");

    EXPECT_NO_THROW({
        auto response = client.get();
        ASSERT_FALSE(response.is_null()) << "Response should not be null";
        ASSERT_TRUE(response.is_object()) << "Response should be a valid JSON object";
    });
}

TEST(HTTPSClientTest, InvalidHost) {
    HTTPSClient client;
    client.setHost("invalid.host");
    client.setEndpoint("/invalid");

    EXPECT_THROW({
        client.get();
    }, HTTPException);
}

TEST(HTTPSClientTest, MissingHost) {
    HTTPSClient client;
    client.setEndpoint("/");

    EXPECT_THROW({
        client.get();
    }, HTTPException);
}
