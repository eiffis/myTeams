#include "server/Server.hpp"

int main(int ac, char **argv)
{
    if (ac == 2 && std::string(argv[1]) == "--help"){
        std::string msg = "USAGE: ./myteams_server port\n\nport is the port number on which the server socket listens.\n";
        std::cout << msg;
        return 0;
    }
    if (ac != 2){
        std::string msg = "USAGE: ./myteams_server port\n\nport is the port number on which the server socket listens.\n";
        std::cout << msg;
        return 84;
    }
    
    try {
        Server::Server serv(std::stoi(argv[1]));
        serv.runServer();
    } catch(const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 84;
    }
    return 0;
}
