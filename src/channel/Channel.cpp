#include "Channel.hpp"

Channel::Channel(std::string channelName, std::string channelDesc, std::string parentUuid)
{
    std::memset(_name, 0, MAX_NAME_LENGTH);
    std::memset(_message, 0, MAX_DESCRIPTION_LENGTH);
    std::memset(_parentTeamUuid, 0, 37);
    std::strncpy(_name, channelName.c_str(), MAX_NAME_LENGTH - 1);
    std::strncpy(_message, channelDesc.c_str(), MAX_DESCRIPTION_LENGTH - 1);
    std::strncpy(_parentTeamUuid, parentUuid.c_str(), 36);
    uuid_generate(_uuid.uuid);
}

bool Channel::store(std::ofstream &out)
{
    if (out.bad()) 
        return false;
    out.write(reinterpret_cast<const char *>(this), sizeof(Channel));
    return !out.fail();
}

bool Channel::load(std::ifstream &in)
{
    if (in.bad())
        return false;
    in.read(reinterpret_cast<char *>(this), sizeof(Channel));
    return !in.fail();
}