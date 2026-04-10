#pragma once
#include <string>
#include <vector>
#include <iostream>

class Parser {
    public:
        Parser() = default;
        ~Parser() = default;
        void parseCommands(std::string buffer);
        void parseArgs(std::string args);
        std::string getCommand() const;
        std::vector<std::string> getArgs() const;
    private:
        std::string _command;
        std::vector<std::string> _arguments;
};
