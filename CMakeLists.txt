cmake_minimum_required(VERSION 3.30)
project(bluesky_feed)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

find_package(OpenSSL REQUIRED)
add_definitions(-DCPPHTTPLIB_OPENSSL_SUPPORT)
target_link_libraries(bluesky_feed OpenSSL::SSL OpenSSL::Crypto)