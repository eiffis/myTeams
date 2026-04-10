#include "user/User.hpp"

User::User(std::string username)
{
    _username = username;
    uuid_generate(_uuid.uuid);    
}

User::~User()
{
    _loggedIn = false;
}

std::string User::getUsername() const 
{
    return _username;
}

myUuid User::getUuid() const
{
    return _uuid;
}

bool User::isLoggedIn() const
{
    return _loggedIn;
}
