#pragma once 
#include <string>
#include <uuid/uuid.h>
#include <memory>

class User {
    public:
        User(std::string username);
        ~User();
        std::string getUsername() const;
        std::string getUuid() const;
        bool isLoggedIn() const;

    private:
        uuid_t _uuid;
        std::string _username;
        bool _loggedIn;
};
