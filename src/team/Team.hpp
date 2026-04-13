#pragma once

#include "Constants.hpp"
#include "channel/Channel.hpp"
#include <algorithm>

class Team
{
private:
    char _name[MAX_NAME_LENGTH];
    char _message[MAX_DESCRIPTION_LENGTH];
    myUuid _uuid;
    std::vector<myUuid> _subscribedUsers;
public:
    Team() = default;
    Team(std::string teamName, std::string teamDesc);
    ~Team() = default;

    const char *getName() const { return _name; };
    myUuid getUuid() const { return _uuid; };
    bool store(std::ofstream &out);
    bool load(std::ifstream &in);
    void subscribeUser(myUuid userUuid);
    bool isUserSubscribed(std::string userUuidStr) const;
    void unsubscribeUser(std::string userUuidStr);
};
