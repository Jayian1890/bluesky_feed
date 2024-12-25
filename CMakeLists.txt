cmake_minimum_required(VERSION 3.30)
project(bluesky_feed)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Your executable or library
add_executable(bluesky_feed
    main.cpp
        actor/getProfile.cpp
        config/Settings.cpp
        config/Settings.h
        network/https_client.cpp
        network/https_client.h
        handlers/CommandHandler.cpp
        handlers/CommandHandler.h
        network/OAuthClient.hpp
        tools/url_encoder.cpp
        tools/url_encoder.h
        tools/logging.hpp
)

# Find OpenSSL
find_package(OpenSSL REQUIRED)

# Include directories for OpenSSL
include_directories(${OPENSSL_INCLUDE_DIR})

# Link OpenSSL libraries
target_link_libraries(bluesky_feed OpenSSL::SSL OpenSSL::Crypto)

add_definitions(-DCPPHTTPLIB_OPENSSL_SUPPORT)

# Enable testing
enable_testing()

# Fetch GoogleTest
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

# Link test executable
add_executable(https_client_test tests/test_https_client.cpp)
target_link_libraries(https_client_test PRIVATE HTTPSClient gtest_main gtest)