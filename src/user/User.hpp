#pragma once 
#include <string>
#include <uuid/uuid.h>
#include "Models.hpp"
#include <memory>

class User {
    public:
        User(std::string username);
        ~User();
        std::string getUsername() const;
        myUuid getUuid() const;
        bool isLoggedIn() const;

    private:
        myUuid _uuid;
        std::string _username;
        bool _loggedIn;
};
