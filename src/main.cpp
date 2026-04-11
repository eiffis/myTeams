#include "server/Server.hpp"

int main(int ac, char **argv)
{
    if (ac != 2)
        throw std::runtime_error("Error on arguments");
    
    try {
        Server::Server serv(std::stoi(argv[1]));
        serv.runServer();
    } catch(const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 84;
    }
    return 0;
}
