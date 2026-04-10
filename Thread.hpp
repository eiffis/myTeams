#pragma once

#include <uuid/uuid.h>
#include <vector>
#include <string>
#include <fstream>
#include "Constants.hpp"
#include "Reply.hpp"
#include "Thread.hpp"

class Thread
{
private:
    char _name[MAX_NAME_LENGTH];
    char _messsage[MAX_DESCRIPTION_LENGTH];
    std::string _uuid;
    std::vector<Reply> _replies;
public:
    Thread(/* args */);
    ~Thread();

    char *getName() { return _name; };
    std::string &getUuid() { return _uuid; };
    bool store(std::ofstream &out);
    bool load(std::ifstream &in);
};
