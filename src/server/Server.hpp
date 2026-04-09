#pragma once
#include <netinet/in.h>
#include <stdexcept>
#include <algorithm>
#include <string>
#include <iostream>
#include <poll.h>
#include <unistd.h>
#include <vector>

#define NO_TIMEOUT -1

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
            size_t _nbFds;
    };
}
