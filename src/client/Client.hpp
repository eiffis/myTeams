#pragma once
#include "server/Server.hpp"


namespace Client {
    class Client {
        public:
            Client(std::string adress, std::string port);
            ~Client() = default;
            void runClient();
        private:
            std::vector<struct pollfd> _fds;
            std::string _adress;
            std::string _port;
    };
}
