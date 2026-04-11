#include "client/Client.hpp"

int main(int ac, char **argv)
{
    if (ac != 3)
        throw std::runtime_error("Error on arguments.");
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
