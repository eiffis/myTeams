#pragma once

#include "Constants.hpp"
#include "channel/Channel.hpp"

class Team
{
private:
    char _name[MAX_NAME_LENGTH];
    char _message[MAX_DESCRIPTION_LENGTH];
    myUuid _uuid;
    std::vector<myUuid> _subscribedUsers;
    std::vector<Channel> _channels;
public:
    Team() = default;
    Team(std::string teamName, std::string teamDesc);
    ~Team() = default;

    const char *getName() { return _name; };
    myUuid getUuid() const { return _uuid; };
    bool store(std::ofstream &out);
    bool load(std::ifstream &in);
};