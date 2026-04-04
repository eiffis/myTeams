#pragma once
#include <netinet/in.h>
#include <stdexcept>
#include <algorithm>
#include <unistd.h>

namespace Server {
    class Server {
        public:
            Server(int port);
            ~Server();
        private:
            int _serverFD;
            int _port;
            int _nbClients;
            struct sockaddr_in _serverAddress;
    };
}