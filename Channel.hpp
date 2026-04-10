#pragma once

#include "Constants.hpp"
#include "Thread.hpp"

class Channel
{
private:
    char _name[MAX_NAME_LENGTH];
    char _message[MAX_DESCRIPTION_LENGTH];
    myUuid _uuid;
    std::vector<Thread> _threads;
public:
    Channel(/* args */);
    ~Channel();

    char *getName() { return _name; };
    uuid_t &getUuid() { return _uuid.uuid; };
    bool store(std::ofstream &out);
    bool load(std::ifstream &in);
};
