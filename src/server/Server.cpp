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
    _commandsTab["/logout"] = &Server::logoutCommand;
    _commandsTab["/help"] = &Server::helpCommand;
    _commandsTab["/users"] = &Server::usersCommand;
    _commandsTab["/user"] = &Server::userCommand;
    _commandsTab["/send"] = &Server::sendCommand;
    //Fair toute l'initialisation ici (c moche oui)
}

Server::Server::~Server()
{
    close(_serverFD);
}

void Server::Server::sendCommand(int clientFD, const std::vector<std::string> &arguments)
{
    if (arguments.size() != 2){
        write(clientFD, "INVALID_ARGS.\n", 15);
        return;
    }
    if (arguments[1].length() > MAX_BODY_LENGTH){
        write(clientFD, "400 Message body too long.\n", 28);
        return;
    }
    std::string uuid = arguments.front();
    auto itReceiver = std::find_if(_users.begin(), _users.end(), [&uuid](const User& u) {
        char uuidStr[37];
        uuid_unparse(u.getUuid().uuid, uuidStr);
        return std::string(uuidStr) == uuid;
    });
    auto itSender = std::find_if(_users.begin(), _users.end(), [&clientFD](const User& u) {
        return u.getFd() == clientFD;
    });
    if (itReceiver != _users.end() && itSender != _users.end()) {
        int receiverFD = itReceiver->getFd();
        char receiverUiid[37];
        char senderUuid[37];
        uuid_unparse(itSender->getUuid().uuid, senderUuid);
        uuid_unparse(itReceiver->getUuid().uuid, receiverUiid);
        write(receiverFD, arguments[1].c_str(), strlen(arguments[1].c_str()));
        write(receiverFD, "\n", 1);
        server_event_private_message_sended(senderUuid, receiverUiid, arguments[1].c_str());
        std::string message = "200 message sent to: " + itReceiver->getUsername() + '\n';
        write(clientFD, message.c_str(), strlen(message.c_str()));
    } else {
        write(clientFD, "400 User not found.\n", 21);
        return;
    }
}

void Server::Server::userCommand(int clientFD, const std::vector<std::string> &arguments)
{
    if (arguments.size() != 1) {
        write(clientFD, "INVALID_ARGS.\n", 15);
        return;
    }
    std::string uuid = arguments.front();
    auto it = std::find_if(_users.begin(), _users.end(), [&uuid](const User& u) {
        char uuidStr[37];
        uuid_unparse(u.getUuid().uuid, uuidStr);
        return std::string(uuidStr) == uuid;
    });
    if (it != _users.end()) {
        std::string status = it->isLoggedIn() ? "1" : "0";
        char uuidStr[37];
        uuid_unparse(it->getUuid().uuid, uuidStr);
        std::string msg = "EVENT_USER \"" + std::string(uuidStr) + "\" \"" + it->getUsername() + "\" \"" + status + "\"\n";
        write(clientFD, msg.c_str(), msg.length());
    } else {
        std::string msg = "EVENT_USER_DON'T_EXISTS \"" + uuid + "\"\n";
        return;
    }
}

void Server::Server::usersCommand(int clientFD, const std::vector<std::string> &arguments)
{
    if (arguments.size() > 0){
        write(clientFD, "INVALID_ARGS.\n", 15);
        return;
    }
    for (const auto& u : _users) {
        char uuidStr[37];
        uuid_unparse(u.getUuid().uuid, uuidStr);
        std::string status = u.isLoggedIn() ? "1" : "0";
        std::string msg = "EVENT_USERS \"" + std::string(uuidStr) + "\" \"" + u.getUsername() + "\" \"" + status + "\"\n";
        write(clientFD, msg.c_str(), msg.length());
    }
    return;
}

void Server::Server::helpCommand(int clientFD, const std::vector<std::string> &arguments)
{
    if (arguments.size() > 0){
        write(clientFD, "INVALID_ARGS.\n", 15);
        return;
    }
    write(clientFD, "Show Help.\n", 12);
    return;
}

void Server::Server::logoutCommand(int clientFD, const std::vector<std::string> &arguments)
{
    if (arguments.size() > 0){
        write(clientFD, "INVALID_ARGS.\n", 24);
        return;
    }
    auto it = std::find_if(_users.begin(), _users.end(), [&clientFD](const User& u) {
        return u.getFd() == clientFD;
    });
    size_t index = std::distance(_users.begin(), it);
    if (it != _users.end()) {
        char uuidStr[37];
        uuid_unparse(_users.at(index).getUuid().uuid, uuidStr);
        server_event_user_logged_out(uuidStr);
        it->setLogState(false);
        it->setFD(-1);
        std::string msg = "EVENT_LOGGED_OUT \"" + std::string(uuidStr) + "\" \"" + it->getUsername() + "\"\n";
        write(clientFD, msg.c_str(), strlen(msg.c_str()));
        return;
    } else {
        std::string msg = "USER NOT FOUND\n";
        write(clientFD, msg.c_str(), strlen(msg.c_str()));
    }
}

void Server::Server::loginCommand(int clientFD, const std::vector<std::string> &arguments)
{
    if (arguments.size() != 1){
        write(clientFD, "INVALID_ARGS.\n", 15);
        return;
    }
    std::string username = arguments.front();
    auto it = std::find_if(_users.begin(), _users.end(), [&username](const User& u) {
        return u.getUsername() == username;
    });
    if (it != _users.end()){
        it->setFD(clientFD);
        it->setLogState(true);
        char uuidItStr[37];
        uuid_unparse(it->getUuid().uuid, uuidItStr);
        server_event_user_logged_in(uuidItStr);
        std::string msg = "EVENT_LOGGED_IN \"" + std::string(uuidItStr) + "\" \"" + username + "\"\n";
        write(clientFD, msg.c_str(), strlen(msg.c_str()));
        return;
    }
    User newUser(username, clientFD);
    myUuid userUuid = newUser.getUuid();
    char uuidStr[37];
    uuid_unparse(userUuid.uuid, uuidStr);
    server_event_user_created(uuidStr, newUser.getUsername().c_str());
    _users.push_back(newUser);
    std::string msg = "EVENT_USER_CREATED \"" + std::string(uuidStr) + "\" \"" + username + "\"\n";
    write(clientFD, msg.c_str(), strlen(msg.c_str()));
    return;
}

void Server::Server::handleCommand(Parser &parser, int clientFD)
{
    if (_commandsTab.find(parser.getCommand()) != _commandsTab.end()){
        (this->*(_commandsTab[parser.getCommand()]))(clientFD, parser.getArgs());
    } else {
        write(clientFD, "UNRECOGNIZED COMMAND\n", 22);
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
                    _clientBuffers[newClient] = "";
                }
            }
            else {
                bytesRead = read(_fds[i].fd, buffer, sizeof(buffer) - 1);
                if (bytesRead <= 0) {
                    if (bytesRead == 0)
                        std::cout << "USER DISCONNECTED" << std::endl;
                    else continue;
                    int fd = _fds[i].fd;
                    auto it = std::find_if(_users.begin(), _users.end(), [&fd](const User& u) {
                        return u.getFd() == fd;
                    });
                    if (it != _users.end()){
                        char uuidItStr[37];
                        uuid_unparse(it->getUuid().uuid, uuidItStr);
                        server_event_user_logged_out(uuidItStr);
                        it->setFD(-1);
                        it->setLogState(false);
                    }
                    _clientBuffers.erase(fd);
                    close(_fds[i].fd);
                    _fds.erase(_fds.begin() + i);
                    _nbFds--;
                    _nbClients--;
                    i--;
                    continue;
                }
                buffer[bytesRead] = '\0';
                _clientBuffers[_fds[i].fd] += buffer;
                size_t pos;
                while ((pos = _clientBuffers[_fds[i].fd].find('\n')) != std::string::npos) {
                    std::string fullCommand = _clientBuffers[_fds[i].fd].substr(0, pos + 1);
                    _clientBuffers[_fds[i].fd].erase(0, pos + 1);
                    parser.parseCommands(fullCommand);
                    if (!parser.getCommand().empty())
                        handleCommand(parser, _fds[i].fd);
                }
            }
        }
    }
    return;
}
