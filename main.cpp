#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include "actor/getProfile.cpp"


class CommandHandler {
public:
    static void executeCommand(const std::string& command, const std::vector<std::string>& args) {
        if (command == "parse") {
            if (args.empty()) {
                std::cout << "Error: parse requires a JSON string as argument." << std::endl;
                return;
            }
            JSON().parse(args[0]);
        } else if (command == "getprofile") {
            const getProfile profile(args[0]);
            std::cout << profile.generate() << std::endl;
        } else if (command == "help") {
            printHelp();
        } else {
            std::cout << "Unknown command: " << command << std::endl;
        }
    }

    static void printHelp() {
        std::cout << "Available commands:" << std::endl;
        //std::cout << "  parse <json_string>  - Parses a JSON string" << std::endl;
        std::cout << "  getprofile           - Returns details for the Profile" << std::endl;
        std::cout << "  help                 - Shows this help message" << std::endl;
        std::cout << "  exit                 - Exit the program" << std::endl;
    }
};

// Helper function to split input into command and arguments
std::pair<std::string, std::vector<std::string>> parseInput(const std::string& input) {
    std::istringstream stream(input);
    std::string command;
    std::vector<std::string> args;
    std::string arg;

    stream >> command; // First word is the command
    while (stream >> arg) { // Remaining words are arguments
        args.push_back(arg);
    }

    return {command, args};
}

int main() {
    std::string input;

    std::cout << "Welcome to IP-Feed Console!" << std::endl;
    std::cout << "Type 'help' for a list of commands." << std::endl;

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input); // Get user input

        auto [command, args] = parseInput(input); // Split input into command and arguments

        if (command == "exit") {
            std::cout << "Goodbye!" << std::endl;
            break;
        }

        CommandHandler::executeCommand(command, args); // Handle the command
    }

    return 0;
}