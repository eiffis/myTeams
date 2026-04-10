#pragma once

#include "Constants.hpp"
#include "Channel.hpp"

class Team
{
private:
    char _name[MAX_NAME_LENGTH];
    char _message[MAX_DESCRIPTION_LENGTH];
    std::string _uuid;
    std::vector<std::string> _subscribedUsers;
    std::vector<Channel> _channels;
public:
    Team(/* args */);
    ~Team();

    char *getName() { return _name; };
    std::string &getUuid() { return _uuid; };
    bool store(std::ofstream &out);
    bool load(std::ifstream &in);
};