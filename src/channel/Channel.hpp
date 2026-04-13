#pragma once

#include <cstring>
#include "Constants.hpp"
#include "thread/Thread.hpp"

class Channel
{
private:
    char _name[MAX_NAME_LENGTH];
    char _message[MAX_DESCRIPTION_LENGTH];
    myUuid _uuid;
    char _parentTeamUuid[37];

public:
    Channel() = default;
    Channel(std::string channelName, std::string channelDesc, std::string _parentNameUuid);
    ~Channel() = default;

    const char *getName() const { return _name; };
    const char *getParentTeamUuid() const { return _parentTeamUuid;};
    myUuid getUuid() const{ return _uuid; };
    bool store(std::ofstream &out);
    bool load(std::ifstream &in);
};
