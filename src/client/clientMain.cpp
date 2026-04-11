#include "client/Client.hpp"

int main(int ac, char **argv)
{
    if (ac != 3){
        std::string msg = "USAGE: ./myteams_cli ip port\n\nip is the server ip address on which the server socket listens\nport is the port number on which the server socket listens\n";
        std::cout << msg;
        return 84;
    }
    std::string ip(argv[1]);
    std::string port(argv[2]);
    try {
        Client::Client client(ip, port);
        client.runClient();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 84;
    }
    return 0;
}
