#include "Thread.hpp"

Thread::Thread(std::string threadName, std::string threadDesc, std::string channelUuid, std::string userUuid)
{
    std::memset(_name, 0, MAX_NAME_LENGTH);
    std::memset(_message, 0, MAX_DESCRIPTION_LENGTH);
    std::memset(_channelUuid, 0, 37);
    std::memset(_userUuid, 0, 37);
    std::strncpy(_name, threadName.c_str(), MAX_NAME_LENGTH - 1);
    std::strncpy(_message, threadDesc.c_str(), MAX_DESCRIPTION_LENGTH - 1);
    std::strncpy(_channelUuid, channelUuid.c_str(), 36);
    std::strncpy(_userUuid, userUuid.c_str(), 36);
    uuid_generate(_uuid.uuid);
    _timestamp = time(&_timestamp);
}

bool Thread::store(std::ofstream &out)
{
    if (out.bad())
        return false;
    out.write(_name, MAX_NAME_LENGTH);
    out.write(_message, MAX_DESCRIPTION_LENGTH);
    out.write(_channelUuid, 37);
    out.write(_userUuid, 37);
    out.write(reinterpret_cast<const char *>(&_uuid), sizeof(myUuid));
    out.write(reinterpret_cast<const char *>(&_timestamp), sizeof(time_t));
    size_t replylen = _replies.size();
    out.write(reinterpret_cast<const char *>(&replylen), sizeof(size_t));
    out.write(reinterpret_cast<const char *>(_replies.data()), replylen * sizeof(Reply));
    return !out.fail();
}

bool Thread::load(std::ifstream &in)
{
    if (in.bad())
        return false;
    in.read(_name, MAX_NAME_LENGTH);
    in.read(_message, MAX_DESCRIPTION_LENGTH);
    in.read(_channelUuid, 37);
    in.read(_userUuid, 37);
    in.read(reinterpret_cast<char *>(&_uuid), sizeof(myUuid));
    in.read(reinterpret_cast<char *>(&_timestamp), sizeof(time_t));
    size_t replylen = 0;
    in.read(reinterpret_cast<char *>(&replylen), sizeof(size_t));
    if (replylen) {
        _replies.resize(replylen);
        in.read(reinterpret_cast<char *>(_replies.data()), replylen * sizeof(Reply));
    }
    return !in.fail();
}

time_t Thread::getTimestamp() const
{
    return _timestamp;
}

void Thread::addReply(const Reply &reply)
{
    _replies.push_back(reply);
}