#pragma once

#include "Constants.hpp"
#include "Channel.hpp"

class Team
{
private:
    char _name[MAX_NAME_LENGTH];
    char _message[MAX_DESCRIPTION_LENGTH];
    uuid_t _uuid;
    std::vector<uuid_t> _subscribedUsers;
    std::vector<Channel> _channels;
public:
    Team(/* args */);
    ~Team();

    char *getName() { return _name; };
    uuid_t &getUuid() { return _uuid; };
    bool store(std::ofstream &out);
    bool load(std::ifstream &in);
};