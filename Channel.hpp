#pragma once

#include "Constants.hpp"
#include "Thread.hpp"

class Channel
{
private:
    char _name[MAX_NAME_LENGTH];
    char _message[MAX_DESCRIPTION_LENGTH];
    std::string _uuid;
    std::vector<Thread> _threads;
public:
    Channel(/* args */);
    ~Channel();

    char *getName() { return _name; };
    std::string &getUuid() { return _uuid; };
    bool store(std::ofstream &out);
    bool load(std::ifstream &in);
};
