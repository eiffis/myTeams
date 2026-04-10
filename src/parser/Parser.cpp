#include "parser/Parser.hpp"

void Parser::parseArgs(std::string args)
{
    std::cout << "Les arguments de la commandes: " << args << std::endl;
    int start = 0;
    int end = 0;
    char delimiter = ' ';

    while ((start = args.find_first_not_of(delimiter, end)) != std::string::npos) {
        end = args.find(delimiter, start);
        _arguments.push_back(args.substr(start, end - start));
    }
    for (auto arg : _arguments) {
        std::cout << arg << std::endl;
    }
}

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
        std::cout << "Nom de la commande: " << _command << std::endl;
        parseArgs(args);
    }
}
