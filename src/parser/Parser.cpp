#include "parser/Parser.hpp"

void Parser::parseArgs(std::string args)
{
    int start = 0;
    int end = 0;

    for (int i = 0; (size_t)i < args.size(); i++) {
        if (args[i] == ' ')
            continue;
        if (args[i] == '"'){
            i++;
            start = i;
            end = args.find('"', start);
            if ((size_t)end != std::string::npos) {
                _arguments.push_back(args.substr(start, end - start));
                i = end;
            } else {
                _command = "INVALID";
                _arguments.clear();
                break;
            }
        }
    }
}

void Parser::parseCommands(std::string buffer)
{
    _command.clear();
    _arguments.clear();
    buffer.erase(buffer.find_last_not_of(" \n\r\t") + 1);
    size_t separation;
    std::string args;

    if (buffer.empty())
        return;
    separation = buffer.find(" ");
    if (separation == std::string::npos){
        _command = buffer;
        _arguments.clear();
    } else {
        _command = buffer.substr(0, separation);
        args = buffer.substr(separation + 1);
        _arguments.clear();
        parseArgs(args);
    }
}

std::string Parser::getCommand() const 
{
    return _command;
}

std::vector<std::string> Parser::getArgs() const
{
    return _arguments;
}
