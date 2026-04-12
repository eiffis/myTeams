#include "user/User.hpp"

User::User(std::string username, int fd)
{
    _username = username;
    _fd = fd;
    _loggedIn = true;
    _currentContext = NONE;
    uuid_generate(_uuid.uuid);    
}

User::~User()
{
    _loggedIn = false;
}

int User::getFd() const 
{
    return _fd;
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

void User::setFD(int FD)
{
    _fd = FD;
}

void User::setLogState(bool state)
{
    _loggedIn = state;
}

void User::setContext(Context context)
{
    _currentContext = context;
}

void User::setTeamUuid(std::string teamUuid)
{
    _teamUuid = teamUuid;
}

void User::setChannelUuid(std::string channelUuid)
{
    _channelUuid = channelUuid;
}

void User::setThreadUuid(std::string threadUuid)
{
    _threadUuid = threadUuid;
}

Context User::getContext() const
{
    return _currentContext;
}
