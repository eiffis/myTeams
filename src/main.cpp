#include "server/Server.hpp"

int main(int ac, char **argv)
{
    if (ac != 2)
        throw std::runtime_error("Error on arguments");
    Server::Server serv(std::stoi(argv[1]));
    
    serv.runServer();
}
