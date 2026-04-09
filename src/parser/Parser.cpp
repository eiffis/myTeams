#include "Parser.hpp"

void Parser::parseCommands(std::string buffer)
{
    buffer.erase(buffer.find_last_not_of("\n") + 1);
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
        // parser les args pour les mettre dans le vecteur et faire attention aux quotes 
    }
}
