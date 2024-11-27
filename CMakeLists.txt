cmake_minimum_required(VERSION 3.30)
project(bluesky_feed)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(bluesky_feed
    main.cpp
        actor/getProfile.cpp
        tools/JSON.cpp
        config/Settings.cpp
        network/HTTPSClient.cpp
        handlers/CommandHandler.cpp
)

find_package(CURL REQUIRED)
target_link_libraries(bluesky_feed CURL)
