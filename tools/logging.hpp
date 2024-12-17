//
// Created by jayian on 12/3/24.
// Copyright (c) 2024 Interlaced Pixel. All rights reserved.
//

#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <fstream>
#include <string>
#include <mutex>
#include <iostream>
#include <optional>
#include <iomanip>
#include <sstream>

class Logging {
public:
    static void info(const std::string &message, const bool toConsole = true) {
        log("[INFO] " + message, std::cout, toConsole);
    }

    static void error(const std::string &message, const bool toConsole = true) {
        log("[ERROR] " + message, std::cerr, toConsole);
    }

    static void debug(const std::string &message, const bool toConsole = true) {
        log("[DEBUG] " + message, std::cout, toConsole);
    }

private:
    inline static std::optional<std::ofstream> logFile;
    inline static std::mutex logMutex;

    static void log(const std::string &message, std::ostream &outputStream, const bool toConsole) {
        std::lock_guard lock(logMutex);

        const auto timestampFile = getCurrentTimestampFile();
        const auto timestampConsole = getCurrentTimestampConsole();

        // Lazy initialization of the log file
        if (!logFile) {
            getLogFile() = std::ofstream("log.txt", std::ios::app);
        }

        // Log to file if it is open
        if (logFile && logFile->is_open()) {
            *logFile << "[" << timestampFile << "] " << message << std::endl;
        }

        // Log to console if enabled
        if (toConsole) {
            outputStream << "[" << timestampConsole << "] " << message << std::endl;
        }
    }

    static std::optional<std::ofstream>& getLogFile() {
        return logFile;
    }

    static std::string getCurrentTimestampFile() {
        const auto now = std::chrono::system_clock::now();
        const auto time = std::chrono::system_clock::to_time_t(now);
        const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;

        // Use localtime_r for thread safety
        tm localTime{};
        localtime_r(&time, &localTime);

        std::ostringstream oss;
        oss << std::put_time(&localTime, "%m:%d:%Y %H:%M:%S")
            << '.' << std::setfill('0') << std::setw(3) << milliseconds.count();
        return oss.str();
    }

    static std::string getCurrentTimestampConsole() {
        const auto now = std::chrono::system_clock::now();
        const auto time = std::chrono::system_clock::to_time_t(now);

        // Use localtime_r for thread safety
        tm localTime{};
        localtime_r(&time, &localTime);

        std::ostringstream oss;
        oss << std::put_time(&localTime, "%H:%M:%S");
        return oss.str();
    }
};

#endif // LOGGING_HPP