#pragma once
#include "server/Server.hpp"
extern "C" {
    #include "logging_client.h"
}

namespace Client {
    class Client {
        public:
            Client(std::string adress, std::string port);
            ~Client() = default;
            void handleEvent(std::string fullCommand, Parser &parser);
            void runClient();
        private:
            std::vector<struct pollfd> _fds;
            std::string _adress;
            std::string _port;
    };
}
