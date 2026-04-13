#include "Channel.hpp"

Channel::Channel(std::string channelName, std::string channelDesc)
{
    std::memset(_name, 0, MAX_NAME_LENGTH);
    std::memset(_message, 0, MAX_DESCRIPTION_LENGTH);
    std::strncpy(_name, channelName.c_str(), MAX_NAME_LENGTH - 1);
    std::strncpy(_message, channelDesc.c_str(), MAX_DESCRIPTION_LENGTH - 1);
    uuid_generate(_uuid.uuid);
}

bool Channel::store(std::ofstream &out)
{
    if (out.bad())
        return false;
    out.write(_name, MAX_NAME_LENGTH);
    out.write(_message, MAX_DESCRIPTION_LENGTH);
    out.write(reinterpret_cast<const char *>(&_uuid), sizeof(myUuid));
    size_t threadlen = _threads.size();
    out.write(reinterpret_cast<const char *>(&threadlen), sizeof(size_t));
    for (auto &thread : _threads)
    {
        if (!thread.store(out))
            return false;
    }
    if (out.fail())
        return false;
    return true;
}

bool Channel::load(std::ifstream &in)
{
    if (in.bad())
        return false;
    in.read(_name, MAX_NAME_LENGTH);
    in.read(_message, MAX_DESCRIPTION_LENGTH);
    in.read(reinterpret_cast<char *>(&_uuid), sizeof(myUuid));
    size_t threadlen = 0;
    in.read(reinterpret_cast<char *>(&threadlen), sizeof(size_t));
    if (threadlen)
    {
        _threads.resize(threadlen);
        for (auto &thread : _threads)
        {
            if (!thread.load(in))
                return false;
        }
    }
    if (in.fail())
        return false;
    return true;
}