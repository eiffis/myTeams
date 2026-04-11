    #pragma once
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <stdexcept>
    #include <algorithm>
    #include "user/User.hpp"
    #include "parser/Parser.hpp"
    #include "Constants.hpp"
    #include <string>
    #include <cstring>
    #include "team/Team.hpp"
    #include <iostream>
    #include <poll.h>
    #include <unistd.h>
    #include <stdlib.h>
    #include <vector>
    #include <map>
    extern "C" {
        #include "../libs/myteams/logging_server.h"
    }
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
                std::map<int, std::string> _clientBuffers;
                //::vector<Team> _teams; à ajouter plus tard quand la classe Team sera finalisée 
                size_t _nbFds;
                typedef void (Server::*commandHandler)(int clientFD, const std::vector<std::string> &arguments);
                std::map<std::string, commandHandler> _commandsTab;
                // ensuite faire tous les proto des méthodes des commandes /login /logout etc...
                void loginCommand(int clientFD, const std::vector<std::string> &arguments);
                void logoutCommand(int clientFD, const std::vector<std::string> &arguments);
                void helpCommand(int clientFD, const std::vector<std::string> &arguments);
                void usersCommand(int clientFD, const std::vector<std::string> &arguments);
                void userCommand(int clientFD, const std::vector<std::string> &arguments);
                void sendCommand(int clientFD, const std::vector<std::string> &arguments);
        };
    }
