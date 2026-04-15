#pragma once

#include <uuid/uuid.h>
#include <vector>
#include <string>
#include <fstream>
#include <cstring>
#include "Constants.hpp"
#include "Models.hpp"

class Thread
{
private:
    char _name[MAX_NAME_LENGTH];
    char _message[MAX_DESCRIPTION_LENGTH];
    char _userUuid[37];
    myUuid _uuid;
    time_t _timestamp;
    std::vector<Reply> _replies;
    char _channelUuid[37];

public:
    Thread() = default;
    Thread(std::string threadName, std::string threadDesc, std::string channelUuid);
    ~Thread() = default;

    const char *getName() const { return _name; };
    myUuid getUuid() const { return _uuid; };
    const char *getChannelUuid() const { return _channelUuid;};
    const char *getUserUuid() const { return _userUuid; };
    const char *getMessage() const { return _message; };
    const std::vector<Reply>& getReplies() const { return _replies; }
    time_t getTimestamp() const;
    void addReply(const Reply& reply);
    bool store(std::ofstream &out);
    bool load(std::ifstream &in);
};
