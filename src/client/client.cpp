#include "client/client.hpp"

Client::Client::Client(std::string adress, std::string port)
{
    _adress = adress;
    _port = port;
}

void Client::Client::runClient()
{
    int clientFD = socket(AF_INET, SOCK_STREAM, 0);
    struct pollfd stdinFD;
    struct pollfd serverFD;

    if (clientFD < 0)
        throw std::runtime_error("Failed to create client socket");
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(std::stoi(_port));
    if (inet_pton(AF_INET, _adress.c_str(), &server_addr.sin_addr) <= 0)
        throw std::runtime_error("IP adress is invalid.");
    if (connect(clientFD, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        throw std::runtime_error("Failed to connect to the server");
    std::cout << "Connected to the myTeams server: " << _adress << ":" << _port << std::endl;
    stdinFD.fd = STDIN_FILENO;
    stdinFD.events = POLLIN;
    stdinFD.revents = 0;
    _fds.push_back(stdinFD);
    serverFD.fd = clientFD;
    serverFD.events = POLLIN;
    serverFD.revents = 0;
    _fds.push_back(serverFD);
}