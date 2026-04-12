#include "Thread.hpp"

Thread::Thread(std::string threadName, std::string threadDesc)
{
    std::strncpy(_name, threadName.c_str(), MAX_NAME_LENGTH);
    std::strncpy(_messsage, threadDesc.c_str(), MAX_DESCRIPTION_LENGTH);
}

bool Thread::store(std::ofstream &out)
{
    if (out.bad())
        return false;
    out.write(_name, MAX_NAME_LENGTH);
    out.write(_messsage, MAX_DESCRIPTION_LENGTH);
    out.write(reinterpret_cast<const char *>(&_uuid), sizeof(myUuid));
    size_t replylen = _replies.size();
    out.write(reinterpret_cast<const char *>(&replylen), sizeof(size_t));
    out.write(reinterpret_cast<const char *>(_replies.data()), replylen * sizeof(Reply));
    if (out.fail())
        return false;
    return true;
}

bool Thread::load(std::ifstream &in)
{
    if (in.bad())
        return false;
    in.read(_name, MAX_NAME_LENGTH);
    in.read(_messsage, MAX_DESCRIPTION_LENGTH);
    in.read(reinterpret_cast<char *>(&_uuid), sizeof(myUuid));
    size_t replylen = 0;
    in.read(reinterpret_cast<char *>(&replylen), sizeof(size_t));
    if (replylen) {
        _replies.resize(replylen);
        in.read(reinterpret_cast<char *>(_replies.data()), replylen * sizeof(Reply));
    }
    if (in.fail())
        return false;
    return true;
}