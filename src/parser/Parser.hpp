#pragma once
#include <string>
#include <vector>

class Parser {
    public:
        Parser() = default;
        ~Parser() = default;
        void parseCommands(std::string buffer);
    private:
        std::string _command;
        std::vector<std::string> _arguments;
};
