#pragma once 
#include <string>
#include <uuid/uuid.h>
#include "Models.hpp"
#include <memory>

class User {
    public:
        User(std::string username, int fd);
        ~User();
        std::string getUsername() const;
        int getFd() const;
        myUuid getUuid() const;
        void setFD(int FD);
        bool isLoggedIn() const;
        void setLogState(bool state);
        
    private:
        myUuid _uuid;
        int _fd;
        std::string _username;
        bool _loggedIn;
        std::string _currentContext;
};
