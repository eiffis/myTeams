#pragma once
#include <netinet/in.h>
#include <stdexcept>
#include <algorithm>
#include "User.hpp"
#include <string>
#include <cstring>
#include <iostream>
#include <poll.h>
#include <unistd.h>
#include <stdlib.h>
#include <vector>

#define NO_TIMEOUT -1
#define CMD_SIZE 4096
namespace Server {
    class Server {
        public:
            Server(int port);
            ~Server();
            void runServer();
        private:
            int _serverFD;
            int _port;
            int _nbClients;
            struct sockaddr_in _serverAddress;
            std::vector<struct pollfd> _fds;
            std::vector<User> _users;
            size_t _nbFds;
    };
}
