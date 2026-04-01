#pragma once

#include <vector>
#include "Constants.hpp"
#include "Channel.hpp"

class Team
{
private:
    char _name[MAX_NAME_LENGTH];
    char _message[MAX_DESCRIPTION_LENGTH];
    uuid_t _uuid;
    std::vector<uuid_t> _subscribedUsers;
    std::vector<Channel> _threads;
public:
    Team(/* args */);
    ~Team();
};