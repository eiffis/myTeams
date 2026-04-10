#include "server/Server.hpp"

Server::Server::Server(int port)
{
    int opt = 1;
    struct pollfd serverFd;

    _serverFD = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverFD == -1)
        throw std::runtime_error("Error on socket");
    if (setsockopt(_serverFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) < 0)
        throw std::runtime_error("Error on setsockopt");
    _port = port;
    _nbClients = 0;
    _nbFds = 1;
    _serverAddress = {};
    _serverAddress.sin_family = AF_INET;
    _serverAddress.sin_port = htons(port);
    _serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(_serverFD, (struct sockaddr *)&_serverAddress, sizeof(_serverAddress)) < 0)
        throw std::runtime_error("Error on bind");
    if (listen(_serverFD, SOMAXCONN) < 0)
        throw std::runtime_error("Error on listen");
    serverFd.fd = _serverFD;
    serverFd.events = POLLIN;
    serverFd.revents = 0;
    _fds.push_back(serverFd);
    _commandsTab["/login"] = &Server::loginCommand;
    //_commandsTab["/logout"] = &Server::logoutCommand;
    //Fair toute l'initialisation ici (c moche oui)
}

Server::Server::~Server()
{
    close(_serverFD);
}

void Server::Server::loginCommand(int clientFD, const std::vector<std::string> &arguments)
{
    if (arguments.size() != 1){
        write(clientFD, "400 Bad request\n", 17);
        return;
    }
    std::string username = arguments.front();
    auto it = std::find_if(_users.begin(), _users.end(), [&username](const User& u) {
        return u.getUsername() == username;
    });
    if (it != _users.end()){
        std::string msg = "User: " + username + " already exists.\n";
        write(clientFD, msg.c_str(), strlen(msg.c_str()));
        return;
    }
    User newUser(username);
    myUuid userUuid = newUser.getUuid();
    char uuidStr[37];
    uuid_unparse(userUuid.uuid, uuidStr);
    server_event_user_created(uuidStr, newUser.getUsername().c_str());
    _users.push_back(newUser);
    std::string msg = "200 user: " + username + " created.\n";
    write(clientFD, msg.c_str(), strlen(msg.c_str()));
    return;
}

void Server::Server::handleCommand(Parser &parser, int clientFD)
{
    if (_commandsTab.find(parser.getCommand()) != _commandsTab.end()){
        (this->*(_commandsTab[parser.getCommand()]))(clientFD, parser.getArgs());
    } else {
        write(clientFD, "504 Unrecognized Command.\n", 27);
    }
}

void Server::Server::runServer()
{
    int newClient;
    struct pollfd newFd;
    char buffer[4096];
    ssize_t bytesRead;
    Parser parser;
    struct sockaddr_in clientAddress;
    socklen_t clientLen;
    

    while (1) {
        if (poll(_fds.data(), _fds.size(), NO_TIMEOUT) == -1)
            break;
        for (int i = 0; i < (int)_fds.size(); i++) {
            if (!(_fds[i].revents & POLLIN))
                continue;
            if (_fds[i].fd == _serverFD){
                clientLen = sizeof(clientAddress);
                newClient = accept(_serverFD, (struct sockaddr *)&clientAddress, &clientLen);
                if (newClient > 0) {
                    newFd.fd = newClient;
                    newFd.events = POLLIN;
                    newFd.revents = 0;
                    _fds.push_back(newFd);
                    _nbFds++;
                    _nbClients++;
                    write(newClient, "Welcome to myTeams.\n", 21);
                }
            }
            else {
                bytesRead = read(_fds[i].fd, buffer, sizeof(buffer) - 1);
                if (bytesRead <= 0) {
                    if (bytesRead == 0)
                        std::cout << "User disconected" << std::endl;
                    else continue;
                    close(_fds[i].fd);
                    _fds.erase(_fds.begin() + i);
                    i--;
                    continue;
                }
                buffer[bytesRead] = '\0';
                parser.parseCommands(buffer);
                handleCommand(parser, _fds[i].fd);
            }
        }
    }
    return;
}
