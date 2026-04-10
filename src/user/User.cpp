#include "user/User.hpp"

User::User(std::string username)
{
    _username = username;
    uuid_generate(_uuid);    
}

User::~User()
{
    _loggedIn = false;
}

std::string User::getUsername() const 
{
    return _username;
}

std::string User::getUuid() const
{
    std::unique_ptr<char> uuidPtr;
    uuid_unparse(_uuid, uuidPtr.get());
    std::string uuid(uuidPtr.get());
    return uuid;
}

bool User::isLoggedIn() const
{
    return _loggedIn;
}
