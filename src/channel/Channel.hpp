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
    std::vector<Thread> _threads;
public:
    Channel(std::string channelName, std::string channelDesc);
    ~Channel() = default;

    char *getName() { return _name; };
    uuid_t &getUuid() { return _uuid.uuid; };
    bool store(std::ofstream &out);
    bool load(std::ifstream &in);
};
