#include <iostream>
#include "Settings.cpp"
#include "CommandHandler.cpp"


// Helper function to split input into command and arguments
std::pair<std::string, std::vector<std::string>> parseInput(const std::string& input) {
    std::istringstream stream(input);
    std::string command;
    std::vector<std::string> args;
    std::string arg;

    stream >> command;
    while (stream >> arg) {
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