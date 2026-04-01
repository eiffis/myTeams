#pragma once

#include <uuid/uuid.h>
#include <vector>
#include "Constants.hpp"
#include "Thread.hpp"

class Thread
{
private:
    char _name[MAX_NAME_LENGTH];
    char _messsage[MAX_DESCRIPTION_LENGTH];
    uuid_t _uuid;
    std::vector<std::pair<uuid_t, char[MAX_BODY_LENGTH]>> _replies;
public:
    Thread(/* args */);
    ~Thread();
};
