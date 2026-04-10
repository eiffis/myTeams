#pragma once
#include <netinet/in.h>
#include <stdexcept>
#include <algorithm>
#include "user/User.hpp"
#include "parser/Parser.hpp"
#include <string>
#include <cstring>
#include <iostream>
#include <poll.h>
#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include <map>

#define NO_TIMEOUT -1
#define CMD_SIZE 4096
namespace Server {
    class Server {
        public:
            Server(int port);
            ~Server();
            void handleCommand(Parser &parser, int clientFD);
            void runServer();
        private:
            int _serverFD;
            int _port;
            int _nbClients;
            struct sockaddr_in _serverAddress;
            std::vector<struct pollfd> _fds;
            std::vector<User> _users;
            size_t _nbFds;
            typedef void (Server::*commandHandler)(int clientFD, const std::vector<std::string> &arguments);
            std::map<std::string, commandHandler> _commandsTab;
            // ensuite faire tous les proto des méthodes des commandes /login /logout etc...
            void loginCommand(int clientFD, const std::vector<std::string> &arguments);
            void logoutCommand(int clientFD, const std::vector<std::string> &arguments);
    };
}
