#include "Team.hpp"

bool Team::store(std::ofstream &out)
{
    if (out.bad())
        return false;
    out.write(_name, MAX_NAME_LENGTH);
    out.write(_message, MAX_DESCRIPTION_LENGTH);
    out.write(reinterpret_cast<const char *>(&_uuid), sizeof(myUuid));
    size_t userlen = _subscribedUsers.size();
    out.write(reinterpret_cast<const char *>(&userlen), sizeof(size_t));
    out.write(reinterpret_cast<const char *>(_subscribedUsers.data()), userlen * sizeof(myUuid));
    size_t channellen = _channels.size();
    out.write(reinterpret_cast<const char *>(&channellen), sizeof(size_t));
    for (auto &channel : _channels)
    {
        if (!channel.store(out))
            return false;
    }
    if (out.fail())
        return false;
    return true;
}

bool Team::load(std::ifstream &in)
{
    if (in.bad())
        return false;
    in.read(_name, MAX_NAME_LENGTH);
    in.read(_message, MAX_DESCRIPTION_LENGTH);
    in.read(reinterpret_cast<char *>(&_uuid), sizeof(myUuid));
    size_t userlen = 0;
    in.read(reinterpret_cast<char *>(&userlen), sizeof(size_t));
    if (userlen)
    {
        _subscribedUsers.resize(userlen);
        in.read(reinterpret_cast<char *>(_subscribedUsers.data()), userlen * sizeof(myUuid));
    }
    size_t channellen = 0;
    in.read(reinterpret_cast<char *>(&channellen), sizeof(size_t));
    if (channellen)
    {
        _channels.resize(channellen);
        for (auto &channel : _channels)
        {
            if (!channel.load(in))
                return false;
        }
    }
    if (in.fail())
        return false;
    return true;
}