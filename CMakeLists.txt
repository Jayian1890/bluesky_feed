cmake_minimum_required(VERSION 3.30)
project(bluesky_feed)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform-specific OpenSSL configuration
if(WIN32)
    # Windows OpenSSL setup
    set(OPENSSL_ROOT_DIR "C:/Program Files/OpenSSL/OpenSSL")
    set(OPENSSL_INCLUDE_DIR "C:/Program Files/OpenSSL/include")
    set(OPENSSL_LIBRARIES "C:/Program Files/OpenSSL/lib")
elseif(APPLE)
    # macOS OpenSSL setup - typically uses system libraries or you can install OpenSSL via Homebrew
    find_package(OpenSSL REQUIRED)
elseif(UNIX)
    # Linux OpenSSL setup
    find_package(OpenSSL REQUIRED)
endif()

# Your executable or library
add_executable(bluesky_feed
        main.cpp
        actor/getProfile.cpp
        config/Settings.cpp
        config/settings.hpp
        network/https_client.cpp
        network/https_client.h
        handlers/command_handler.cpp
        handlers/command_handler.h
        network/oauth_client.hpp
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

# Add OpenSSL support for cpp-httplib
add_definitions(-DCPPHTTPLIB_OPENSSL_SUPPORT)