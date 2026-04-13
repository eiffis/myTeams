#pragma once 
#include <string>
#include <uuid/uuid.h>
#include "Models.hpp"
#include "Constants.hpp"
#include <memory>

class User {
    public:
        User(std::string username, int fd);
        ~User();
        std::string getUsername() const;
        int getFd() const;
        myUuid getUuid() const;
        void setFD(int FD);
        void setContext(Context context);
        void setTeamUuid(std::string teamUuid);
        void setChannelUuid(std::string channelUuid);
        void setThreadUuid(std::string threadUuid);
        std::string getTeamUuid() const;
        std::string getChannelUuid() const;
        std::string getThreadUuid() const;
        bool isLoggedIn() const;
        void setLogState(bool state);
        Context getContext() const;
        
    private:
        myUuid _uuid;
        int _fd;
        std::string _username;
        bool _loggedIn;
        Context _currentContext;
        std::string _teamUuid;
        std::string _channelUuid;
        std::string _threadUuid;
};
