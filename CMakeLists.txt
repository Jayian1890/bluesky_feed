cmake_minimum_required(VERSION 3.30)
project(bluesky_feed)

set(CMAKE_CXX_STANDARD 14)

add_executable(bluesky_feed
    main.cpp
        HttpsClient.cpp
        json.cpp
        actor/getProfile.cpp
        Settings.cpp
        CommandHandler.cpp
)

find_package(CURL REQUIRED)
target_link_libraries(bluesky_feed CURL)
