#pragma once

#include <vector>
#include "Constants.hpp"
#include "Thread.hpp"

class Channel
{
private:
    char _name[MAX_NAME_LENGTH];
    char _message[MAX_DESCRIPTION_LENGTH];
    uuid_t _uuid;
    std::vector<Thread> _threads;
public:
    Channel(/* args */);
    ~Channel();
};
