#pragma once

#include <uuid/uuid.h>
#include <vector>
#include <string>
#include <fstream>
#include <cstring>
#include "Constants.hpp"
#include "Models.hpp"
#include "Thread.hpp"

class Thread
{
private:
    char _name[MAX_NAME_LENGTH];
    char _message[MAX_DESCRIPTION_LENGTH];
    myUuid _uuid;
    std::vector<Reply> _replies;
public:
    Thread() = default;
    Thread(std::string threadName, std::string threadDesc);
    ~Thread() = default;

    const char *getName() const { return _name; };
    myUuid getUuid() const { return _uuid; };
    bool store(std::ofstream &out);
    bool load(std::ifstream &in);
};
