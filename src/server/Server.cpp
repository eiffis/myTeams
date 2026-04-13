#include "server/Server.hpp"

Server::Server::Server(int port)
{
    int opt = 1;
    struct pollfd serverFd;

    _serverFD = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverFD == -1)
        throw std::runtime_error("Error on socket");
    if (setsockopt(_serverFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) < 0)
        throw std::runtime_error("Error on setsockopt");
    _port = port;
    _nbClients = 0;
    _nbFds = 1;
    _serverAddress = {};
    _serverAddress.sin_family = AF_INET;
    _serverAddress.sin_port = htons(port);
    _serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(_serverFD, (struct sockaddr *)&_serverAddress, sizeof(_serverAddress)) < 0)
        throw std::runtime_error("Error on bind");
    if (listen(_serverFD, SOMAXCONN) < 0)
        throw std::runtime_error("Error on listen");
    serverFd.fd = _serverFD;
    serverFd.events = POLLIN;
    serverFd.revents = 0;
    _fds.push_back(serverFd);
    _commandsTab["/login"] = &Server::loginCommand;
    _commandsTab["/logout"] = &Server::logoutCommand;
    _commandsTab["/help"] = &Server::helpCommand;
    _commandsTab["/users"] = &Server::usersCommand;
    _commandsTab["/user"] = &Server::userCommand;
    _commandsTab["/send"] = &Server::sendCommand;
    _commandsTab["/messages"] = &Server::messagesCommand;
    _commandsTab["/use"] = &Server::useCommand;
    _commandsTab["/create"] = &Server::createCommand;
    _commandsTab["/subscribe"] = &Server::subscribeCommand;
    _commandsTab["/unsubscribe"] = &Server::unsubscribeCommand;
}

Server::Server::~Server()
{
    close(_serverFD);
}

void Server::Server::unsubscribeCommand(int clientFD, const std::vector<std::string> &arguments)
{
    if (arguments.size() != 1){
        write(clientFD, "INVALID_ARGS.\n", 14);
        return;
    }
    auto itUser = std::find_if(_users.begin(), _users.end(), [&clientFD](const User& u) {
        return u.getFd() == clientFD;
    });
    if (itUser == _users.end() || !itUser->isLoggedIn()) {
        write(clientFD, "UNAUTHORIZED\n", 13);
        return;
    }
    std::string teamUuid = arguments[0];
    auto itTeam = std::find_if(_teams.begin(), _teams.end(), [&teamUuid](const Team& t) {
        char uuidStr[37];
        uuid_unparse(t.getUuid().uuid, uuidStr);
        return std::string(uuidStr) == teamUuid;
    });
    if (itTeam == _teams.end()) {
        std::string msg = "EVENT_UNKNOWN_TEAM \"" + teamUuid + "\"\n";
        write(clientFD, msg.c_str(), msg.length());
        return;
    }
    char userUuidStr[37];
    uuid_unparse(itUser->getUuid().uuid, userUuidStr);
    if (itTeam->isUserSubscribed(userUuidStr)) {
        itTeam->unsubscribeUser(userUuidStr);
    }
    server_event_user_unsubscribed(teamUuid.c_str(), userUuidStr);
    std::string msg = "PERSONAL_UNSUBSCRIBED \"" + std::string(userUuidStr) + "\" \"" + teamUuid + "\"\n";
    write(clientFD, msg.c_str(), msg.length());
}

void Server::Server::subscribeCommand(int clientFD, const std::vector<std::string> &arguments)
{
    if (arguments.size() != 1){
        write(clientFD, "INVALID_ARGS.\n", 14);
        return;
    }
    auto itUser = std::find_if(_users.begin(), _users.end(), [&clientFD](const User& u) {
        return u.getFd() == clientFD;
    });
    if (itUser == _users.end() || !itUser->isLoggedIn()) {
        write(clientFD, "UNAUTHORIZED\n", 13);
        return;
    }
    std::string teamUuid = arguments[0];
    auto itTeam = std::find_if(_teams.begin(), _teams.end(), [&teamUuid](const Team& t) {
        char uuidStr[37];
        uuid_unparse(t.getUuid().uuid, uuidStr);
        return std::string(uuidStr) == teamUuid;
    });
    if (itTeam == _teams.end()) {
        std::string msg = "EVENT_UNKNOWN_TEAM \"" + teamUuid + "\"\n";
        write(clientFD, msg.c_str(), msg.length());
        return;
    }
    char userUuidStr[37];
    uuid_unparse(itUser->getUuid().uuid, userUuidStr);
    if (!itTeam->isUserSubscribed(userUuidStr))
        itTeam->subscribeUser(itUser->getUuid());
    server_event_user_subscribed(teamUuid.c_str(), userUuidStr);
    std::string msg = "PERSONAL_SUBSCRIBED \"" + std::string(userUuidStr) + "\" \"" + teamUuid + "\"\n";
    write(clientFD, msg.c_str(), msg.length());
}


void Server::Server::createCommand(int clientFD, const std::vector<std::string> &arguments)
{
    auto itUser = std::find_if(_users.begin(), _users.end(), [&clientFD](const User& u) {
        return u.getFd() == clientFD;
    });
    if (itUser == _users.end() || !itUser->isLoggedIn()) {
        write(clientFD, "UNAUTHORIZED\n", 13);
        return;
    }
    if (itUser->getContext() == NONE) {
        if (arguments.size() != 2){
            write(clientFD, "INVALID_ARGS.\n", 14);
            return;
        }
        auto itExistingTeam = std::find_if(_teams.begin(), _teams.end(), [&arguments](const Team& t) {
            return std::string(t.getName()) == arguments[0];
        });
        if (itExistingTeam != _teams.end()) {
            write(clientFD, "EVENT_ALREADY_EXIST\n", 20);
        return;
        }
        Team newTeam(arguments[0], arguments[1]);
        char teamUuidStr[37];
        char userUuidStr[37];
        uuid_unparse(itUser->getUuid().uuid, userUuidStr);
        uuid_unparse(newTeam.getUuid().uuid, teamUuidStr);
        server_event_team_created(teamUuidStr, arguments[0].c_str(), userUuidStr);
        _teams.push_back(newTeam);
        std::string personalMsg = "PERSONAL_TEAM_CREATED \"" + std::string(teamUuidStr) + "\" \"" + arguments[0] + "\" \"" + arguments[1] + "\"\n";
        write(clientFD, personalMsg.c_str(), personalMsg.length());
        std::string everyoneMsg = "EVENT_TEAM_CREATED \"" + std::string(teamUuidStr) + "\" \"" + arguments[0] + "\" \"" + arguments[1] + "\"\n";
        for (const auto& user : _users) {
            if (user.isLoggedIn() && user.getFd() != -1) {
                write(user.getFd(), everyoneMsg.c_str(), everyoneMsg.length());
            }
        }
        return;
    }
    if (itUser->getContext() == TEAM && !itUser->getTeamUuid().empty()){
        if (arguments.size() != 2){
            write(clientFD, "INVALID_ARGS.\n", 14);
            return;
        }
        std::string teamUuidSt = itUser->getTeamUuid();
        auto itTeam = std::find_if(_teams.begin(), _teams.end(), [&teamUuidSt](const Team& t) {
            char uuidStr[37];
            uuid_unparse(t.getUuid().uuid, uuidStr);
            return std::string(uuidStr) == teamUuidSt;
        });
        char creatorUuidStr[37];
        uuid_unparse(itUser->getUuid().uuid, creatorUuidStr);
        if (itTeam == _teams.end() || !itTeam->isUserSubscribed(creatorUuidStr)) {
            write(clientFD, "UNAUTHORIZED\n", 13);
            return;
        }
        auto itExistingChannel = std::find_if(_channels.begin(), _channels.end(), [&arguments, &teamUuidSt](const Channel& c) {
            return std::string(c.getName()) == arguments[0] && c.getParentTeamUuid() == teamUuidSt;
        });
        if (itExistingChannel != _channels.end()) {
            write(clientFD, "EVENT_ALREADY_EXIST\n", 20);
            return;
        }
        Channel newChannel(arguments[0], arguments[1], itUser->getTeamUuid());
        char teamUuidStr[37];
        char channelUuidStr[37];
        uuid_t teamUuid;
        uuid_parse(itUser->getTeamUuid().c_str(), teamUuid);
        uuid_unparse(teamUuid, teamUuidStr);
        uuid_unparse(newChannel.getUuid().uuid, channelUuidStr);
        server_event_channel_created(teamUuidStr, channelUuidStr, arguments[0].c_str());
        _channels.push_back(newChannel);
        std::string personalMsg = "PERSONAL_CHANNEL_CREATED \"" + std::string(channelUuidStr) + "\" \"" + arguments[0] + "\" \"" + arguments[1] + "\"\n";
        write(clientFD, personalMsg.c_str(), personalMsg.length());
        std::string everyoneMsg = "EVENT_CHANNEL_CREATED \"" + std::string(channelUuidStr) + "\" \"" + arguments[0] + "\" \"" + arguments[1] + "\"\n";
        if (itTeam != _teams.end()) {
            for (const auto& user : _users) {
                if (user.isLoggedIn() && user.getFd() != -1 && user.getFd() != clientFD) {
                    char targetUuidStr[37];
                    uuid_unparse(user.getUuid().uuid, targetUuidStr);
                    if (itTeam->isUserSubscribed(targetUuidStr))
                        write(user.getFd(), everyoneMsg.c_str(), everyoneMsg.length());
                }
            }
        }
        return;
    }
    if (itUser->getContext() == CHANNEL && !itUser->getTeamUuid().empty() && !itUser->getChannelUuid().empty()){
        if (arguments.size() != 2){
            write(clientFD, "INVALID_ARGS.\n", 14);
            return;
        }
        std::string teamUuidSt = itUser->getTeamUuid();
        auto itTeam = std::find_if(_teams.begin(), _teams.end(), [&teamUuidSt](const Team& t) {
            char uuidStr[37];
            uuid_unparse(t.getUuid().uuid, uuidStr);
            return std::string(uuidStr) == teamUuidSt;
        });
        char creatorUuidStr[37];
        uuid_unparse(itUser->getUuid().uuid, creatorUuidStr);
        if (itTeam == _teams.end() || !itTeam->isUserSubscribed(creatorUuidStr)) {
            write(clientFD, "UNAUTHORIZED\n", 13);
            return;
        }
        std::string channelUuidSt = itUser->getChannelUuid();
        auto itExistingThread = std::find_if(_threads.begin(), _threads.end(), [&arguments, &channelUuidSt](const Thread& t) {
            return std::string(t.getName()) == arguments[0] && std::string(t.getChannelUuid()) == channelUuidSt;
        });
        if (itExistingThread != _threads.end()) {
            write(clientFD, "EVENT_ALREADY_EXIST\n", 20);
            return;
        }
        Thread newThread(arguments[0], arguments[1], itUser->getChannelUuid());
        char threadUuidStr[37];
        char channelUuidStr[37];
        char userUuidStr[37];
        uuid_t channelUuid;
        uuid_parse(itUser->getChannelUuid().c_str(), channelUuid);
        uuid_unparse(channelUuid, channelUuidStr);
        uuid_unparse(itUser->getUuid().uuid, channelUuidStr);
        uuid_unparse(newThread.getUuid().uuid, threadUuidStr);
        server_event_thread_created(channelUuidStr, threadUuidStr, userUuidStr, arguments[0].c_str(), arguments[1].c_str());
        _threads.push_back(newThread);
        std::string teamUuid = itUser->getTeamUuid();
        std::string personalMsg = "PERSONAL_THREAD_CREATED \"" + std::string(threadUuidStr) + "\" \"" + std::string(userUuidStr) + "\" \"" + std::to_string(newThread.getTimestamp()) + "\" \"" + arguments[0] + "\" \"" + arguments[1] + "\"\n";
        write(clientFD, personalMsg.c_str(), personalMsg.length());
        std::string everyoneMsg = "EVENT_THREAD_CREATED \"" + std::string(threadUuidStr) + "\" \"" + std::string(userUuidStr) + "\" \"" + std::to_string(newThread.getTimestamp()) + "\" \"" + arguments[0] + "\" \"" + arguments[1] + "\"\n";
        if (itTeam != _teams.end()) {
            for (const auto& user : _users) {
                if (user.isLoggedIn() && user.getFd() != -1 && user.getFd() != clientFD) {
                    char targetUuidStr[37];
                    uuid_unparse(user.getUuid().uuid, targetUuidStr);
                    if (itTeam->isUserSubscribed(targetUuidStr))
                        write(user.getFd(), everyoneMsg.c_str(), everyoneMsg.length());
                }
            }
        }
        return;
    }
    if (itUser->getContext() == THREAD && !itUser->getTeamUuid().empty() && !itUser->getChannelUuid().empty() && !itUser->getThreadUuid().empty()){
        if (arguments.size() != 1){
            write(clientFD, "INVALID_ARGS.\n", 14);
            return;
        }
        std::string teamUuidSt = itUser->getTeamUuid();
        auto itTeam = std::find_if(_teams.begin(), _teams.end(), [&teamUuidSt](const Team& t) {
            char uuidStr[37];
            uuid_unparse(t.getUuid().uuid, uuidStr);
            return std::string(uuidStr) == teamUuidSt;
        });
        char creatorUuidStr[37];
        uuid_unparse(itUser->getUuid().uuid, creatorUuidStr);
        if (itTeam == _teams.end() || !itTeam->isUserSubscribed(creatorUuidStr)) {
            write(clientFD, "UNAUTHORIZED\n", 13);
            return;
        }
        std::string threadUuid = itUser->getThreadUuid();
        auto itThread = std::find_if(_threads.begin(), _threads.end(), [&threadUuid](const Thread& t) {
            char uuidStr[37];
            uuid_unparse(t.getUuid().uuid, uuidStr);
            return std::string(uuidStr) == threadUuid;
        });
        if (itThread == _threads.end()) {
            write(clientFD, "EVENT_UNKNOWN_THREAD\n", 21);
            return;
        }
        Reply newReply;
        uuid_copy(newReply.user, itUser->getUuid().uuid);
        std::memset(newReply.body, 0, MAX_BODY_LENGTH);
        std::strncpy(newReply.body, arguments[0].c_str(), MAX_BODY_LENGTH - 1);
        itThread->addReply(newReply); 
        char userUuidStr[37];
        uuid_unparse(itUser->getUuid().uuid, userUuidStr);
        time_t timestamp = time(&timestamp);
        server_event_reply_created(threadUuid.c_str(), userUuidStr, newReply.body);
        std::string personalMsg = "PERSONAL_REPLY_CREATED \"" + threadUuid + "\" \"" + std::string(userUuidStr) + "\" \"" + std::to_string(timestamp) + "\" \"" + arguments[0] + "\"\n";
        write(clientFD, personalMsg.c_str(), personalMsg.length());
        std::string teamUuid = itUser->getTeamUuid();
        std::string everyoneMsg = "EVENT_REPLY_RECEIVED \"" + teamUuid + "\" \"" + threadUuid + "\" \"" + std::string(userUuidStr) + "\" \"" + arguments[0] + "\"\n";
        if (itTeam != _teams.end()) {
            for (const auto& user : _users) {
                if (user.isLoggedIn() && user.getFd() != -1 && user.getFd() != clientFD) {
                    char targetUuidStr[37];
                    uuid_unparse(user.getUuid().uuid, targetUuidStr);
                    if (itTeam->isUserSubscribed(targetUuidStr))
                        write(user.getFd(), everyoneMsg.c_str(), everyoneMsg.length());
                }
            }
        }
        return;
    }
}

void Server::Server::useCommand(int clientFD, const std::vector<std::string> &arguments)
{
    auto itUser = std::find_if(_users.begin(), _users.end(), [&clientFD](const User& u) {
        return u.getFd() == clientFD;
    });
    if (itUser == _users.end() || !itUser->isLoggedIn()) {
        write(clientFD, "UNAUTHORIZED\n", 13);
        return;
    }
    if (arguments.size() == 0){
        itUser->setContext(NONE);
        itUser->setTeamUuid("");
        itUser->setChannelUuid("");
        itUser->setThreadUuid("");
        return;
    }
    if (arguments.size() == 1){
        std::string teamUuid = arguments[0];
        auto itTeam = std::find_if(_teams.begin(), _teams.end(), [&teamUuid](const Team& u) {
            char uuidStr[37];
            uuid_unparse(u.getUuid().uuid, uuidStr);
            return std::string(uuidStr) == teamUuid;
        });
        if (itTeam != _teams.end()){
            itUser->setTeamUuid(teamUuid);
            itUser->setChannelUuid("");
            itUser->setThreadUuid("");
            itUser->setContext(TEAM);
            return;
        } else {
            std::string msg = "EVENT_UNKNOWN_TEAM \"" + teamUuid + "\"\n";
            write(clientFD, msg.c_str(), msg.length());
            return;
        }
    }
    if (arguments.size() == 2){
        std::string teamUuid = arguments[0];
        std::string channelUuid = arguments[1];
        auto itTeam = std::find_if(_teams.begin(), _teams.end(), [&teamUuid](const Team& u) {
            char uuidStr[37];
            uuid_unparse(u.getUuid().uuid, uuidStr);
            return std::string(uuidStr) == teamUuid;
        });
        auto itChannel = std::find_if(_channels.begin(), _channels.end(), [&channelUuid](const Channel& u) {
            char uuidStr[37];
            uuid_unparse(u.getUuid().uuid, uuidStr);
            return std::string(uuidStr) == channelUuid;
        });
        if (itTeam != _teams.end() && itChannel != _channels.end()){
            itUser->setTeamUuid(teamUuid);
            itUser->setChannelUuid(channelUuid);
            itUser->setThreadUuid("");
            itUser->setContext(CHANNEL);
            return;
        } else {
            if (itTeam == _teams.end()) {
                std::string msg = "EVENT_UNKNOWN_TEAM \"" + teamUuid + "\"\n";
                write(clientFD, msg.c_str(), msg.length());
            } else {
                std::string msg = "EVENT_UNKNOWN_CHANNEL \"" + channelUuid + "\"\n";
                write(clientFD, msg.c_str(), msg.length());
            }
            return;
        }
    }
    if (arguments.size() == 3){
        std::string teamUuid = arguments[0];
        std::string channelUuid = arguments[1];
        std::string threadUuid = arguments[2];
        auto itTeam = std::find_if(_teams.begin(), _teams.end(), [&teamUuid](const Team& u) {
            char uuidStr[37];
            uuid_unparse(u.getUuid().uuid, uuidStr);
            return std::string(uuidStr) == teamUuid;
        });
        auto itChannel = std::find_if(_channels.begin(), _channels.end(), [&channelUuid](const Channel& u) {
            char uuidStr[37];
            uuid_unparse(u.getUuid().uuid, uuidStr);
            return std::string(uuidStr) == channelUuid;
        });
        auto itThread = std::find_if(_threads.begin(), _threads.end(), [&threadUuid](const Thread& u) {
            char uuidStr[37];
            uuid_unparse(u.getUuid().uuid, uuidStr);
            return std::string(uuidStr) == threadUuid;
        });
        if (itTeam != _teams.end() && itChannel != _channels.end() && itThread != _threads.end()){
            itUser->setTeamUuid(teamUuid);
            itUser->setChannelUuid(channelUuid);
            itUser->setThreadUuid(threadUuid);
            itUser->setContext(THREAD);
            return;
        } else {
            if (itTeam == _teams.end()) {
                std::string msg = "EVENT_UNKNOWN_TEAM \"" + teamUuid + "\"\n";
                write(clientFD, msg.c_str(), msg.length());
            } else if (itChannel == _channels.end()) {
                std::string msg = "EVENT_UNKNOWN_CHANNEL \"" + channelUuid + "\"\n";
                write(clientFD, msg.c_str(), msg.length());
            } else {
                std::string msg = "EVENT_UNKNOWN_THREAD \"" + threadUuid + "\"\n";
                write(clientFD, msg.c_str(), msg.length());
            }
            return;
        }
    }

}

void Server::Server::messagesCommand(int clientFD, const std::vector<std::string> &arguments)
{
    if (arguments.size() != 1){
        write(clientFD, "INVALID_ARGS.\n", 14);
        return;
    }
    std::string uuid = arguments.front();
    auto itReceiver = std::find_if(_users.begin(), _users.end(), [&uuid](const User& u) {
        char uuidStr[37];
        uuid_unparse(u.getUuid().uuid, uuidStr);
        return std::string(uuidStr) == uuid;
    });
    auto itSender = std::find_if(_users.begin(), _users.end(), [&clientFD](const User& u) {
        return u.getFd() == clientFD;
    });
    if (itReceiver != _users.end() && itSender != _users.end()){
        char senderUuidStr[37];
        uuid_unparse(itSender->getUuid().uuid, senderUuidStr);
        char receiverUuidStr[37];
        uuid_unparse(itReceiver->getUuid().uuid, receiverUuidStr);
        for (auto message : _messages) {
            bool me = (message.senderUuid == senderUuidStr && message.receiverUuid == receiverUuidStr);
            bool him = (message.senderUuid == receiverUuidStr && message.receiverUuid == senderUuidStr);
            if (me || him) {
                std::string actualSenderUuid = me ? senderUuidStr : receiverUuidStr;
                std::string msg = "EVENT_MESSAGE_LIST \"" + actualSenderUuid + "\" \"" + std::to_string(message.timestamp) + "\" \"" + message.bodyMessage + "\"\n";
                write(clientFD, msg.c_str(), strlen(msg.c_str()));
            }
        }
    }
}

void Server::Server::sendCommand(int clientFD, const std::vector<std::string> &arguments)
{
    if (arguments.size() != 2){
        write(clientFD, "INVALID_ARGS.\n", 14);
        return;
    }
    if (arguments[1].length() > MAX_BODY_LENGTH){
        write(clientFD, "BODY MESSAGE TOO LONG\n", 23);
        return;
    }
    std::string uuid = arguments.front();
    auto itReceiver = std::find_if(_users.begin(), _users.end(), [&uuid](const User& u) {
        char uuidStr[37];
        uuid_unparse(u.getUuid().uuid, uuidStr);
        return std::string(uuidStr) == uuid;
    });
    auto itSender = std::find_if(_users.begin(), _users.end(), [&clientFD](const User& u) {
        return u.getFd() == clientFD;
    });
    if (itReceiver != _users.end() && itSender != _users.end()) {
        char receiverUiid[37];
        char senderUuid[37];
        messages_t message;
        uuid_unparse(itSender->getUuid().uuid, senderUuid);
        uuid_unparse(itReceiver->getUuid().uuid, receiverUiid);
        server_event_private_message_sended(senderUuid, receiverUiid, arguments[1].c_str());
        std::string msg = "EVENT_MESSAGE_SENT \"" + std::string(senderUuid) + "\" \"" + arguments[1] + "\"\n";
        write(itReceiver->getFd(), msg.c_str(), strlen(msg.c_str()));
        message.senderUuid = senderUuid;
        message.receiverUuid = receiverUiid;
        message.bodyMessage = arguments[1];
        time(&message.timestamp);
        _messages.push_back(message);
        } else {
        std::string msg = "EVENT_USER_DON'T_EXISTS \"" + uuid + "\"\n";
        write(clientFD, msg.c_str(), msg.length());
        return;
    }
}

void Server::Server::userCommand(int clientFD, const std::vector<std::string> &arguments)
{
    if (arguments.size() != 1) {
        write(clientFD, "INVALID_ARGS.\n", 14);
        return;
    }
    std::string uuid = arguments.front();
    auto it = std::find_if(_users.begin(), _users.end(), [&uuid](const User& u) {
        char uuidStr[37];
        uuid_unparse(u.getUuid().uuid, uuidStr);
        return std::string(uuidStr) == uuid;
    });
    if (it != _users.end()) {
        std::string status = it->isLoggedIn() ? "1" : "0";
        char uuidStr[37];
        uuid_unparse(it->getUuid().uuid, uuidStr);
        std::string msg = "EVENT_USER \"" + std::string(uuidStr) + "\" \"" + it->getUsername() + "\" \"" + status + "\"\n";
        write(clientFD, msg.c_str(), msg.length());
    } else {
        std::string msg = "EVENT_USER_DON'T_EXISTS \"" + uuid + "\"\n";
        write(clientFD, msg.c_str(), msg.length());
        return;
    }
}

void Server::Server::usersCommand(int clientFD, const std::vector<std::string> &arguments)
{
    if (arguments.size() > 0){
        write(clientFD, "INVALID_ARGS.\n", 14);
        return;
    }
    for (const auto& u : _users) {
        char uuidStr[37];
        uuid_unparse(u.getUuid().uuid, uuidStr);
        std::string status = u.isLoggedIn() ? "1" : "0";
        std::string msg = "EVENT_USERS \"" + std::string(uuidStr) + "\" \"" + u.getUsername() + "\" \"" + status + "\"\n";
        write(clientFD, msg.c_str(), msg.length());
    }
    return;
}

void Server::Server::helpCommand(int clientFD, const std::vector<std::string> &arguments)
{
    if (arguments.size() > 0){
        write(clientFD, "INVALID_ARGS.\n", 14);
        return;
    }
    write(clientFD, "Show Help.\n", 12);
    return;
}

void Server::Server::logoutCommand(int clientFD, const std::vector<std::string> &arguments)
{
    if (arguments.size() > 0){
        write(clientFD, "INVALID_ARGS.\n", 24);
        return;
    }
    auto it = std::find_if(_users.begin(), _users.end(), [&clientFD](const User& u) {
        return u.getFd() == clientFD;
    });
    size_t index = std::distance(_users.begin(), it);
    if (it != _users.end()) {
        char uuidStr[37];
        uuid_unparse(_users.at(index).getUuid().uuid, uuidStr);
        server_event_user_logged_out(uuidStr);
        std::string msg = "EVENT_LOGGED_OUT \"" + std::string(uuidStr) + "\" \"" + it->getUsername() + "\"\n";
        for (const auto& u : _users) {
            if (u.isLoggedIn() && u.getFd() != -1) {
                write(u.getFd(), msg.c_str(), msg.length());
            }
        }
        it->setLogState(false);
        it->setFD(-1);
        return;
    } else {
        std::string msg = "USER NOT FOUND\n";
        write(clientFD, msg.c_str(), strlen(msg.c_str()));
    }
}

void Server::Server::loginCommand(int clientFD, const std::vector<std::string> &arguments)
{
    if (arguments.size() != 1){
        write(clientFD, "INVALID_ARGS.\n", 14);
        return;
    }
    std::string username = arguments.front();
    auto it = std::find_if(_users.begin(), _users.end(), [&username](const User& u) {
        return u.getUsername() == username;
    });
    if (it != _users.end()){
        it->setFD(clientFD);
        it->setLogState(true);
        char uuidItStr[37];
        uuid_unparse(it->getUuid().uuid, uuidItStr);
        server_event_user_logged_in(uuidItStr);
        std::string msg = "EVENT_LOGGED_IN \"" + std::string(uuidItStr) + "\" \"" + username + "\"\n";
        for (const auto& u : _users) {
            if (u.isLoggedIn() && u.getFd() != -1) {
                write(u.getFd(), msg.c_str(), msg.length());
            }
        }
        return;
    }
    User newUser(username, clientFD);
    myUuid userUuid = newUser.getUuid();
    char uuidStr[37];
    uuid_unparse(userUuid.uuid, uuidStr);
    server_event_user_created(uuidStr, newUser.getUsername().c_str());
    server_event_user_logged_in(uuidStr);
    _users.push_back(newUser);
    std::string msg = "EVENT_USER_CREATED \"" + std::string(uuidStr) + "\" \"" + username + "\"\n";
    write(clientFD, msg.c_str(), strlen(msg.c_str()));
    std::string msg2 = "EVENT_LOGGED_IN \"" + std::string(uuidStr) + "\" \"" + username + "\"\n";
    for (const auto& u : _users) {
        if (u.isLoggedIn() && u.getFd() != -1) {
            write(u.getFd(), msg2.c_str(), msg2.length());
        }
    }
    return;
}

void Server::Server::handleCommand(Parser &parser, int clientFD)
{
    if (_commandsTab.find(parser.getCommand()) != _commandsTab.end()){
        (this->*(_commandsTab[parser.getCommand()]))(clientFD, parser.getArgs());
    } else {
        write(clientFD, "UNRECOGNIZED COMMAND\n", 22);
    }
}

void Server::Server::runServer()
{
    int newClient;
    struct pollfd newFd;
    char buffer[4096];
    ssize_t bytesRead;
    Parser parser;
    struct sockaddr_in clientAddress;
    socklen_t clientLen;
    

    while (1) {
        if (poll(_fds.data(), _fds.size(), NO_TIMEOUT) == -1)
            break;
        for (int i = 0; i < (int)_fds.size(); i++) {
            if (!(_fds[i].revents & POLLIN))
                continue;
            if (_fds[i].fd == _serverFD){
                clientLen = sizeof(clientAddress);
                newClient = accept(_serverFD, (struct sockaddr *)&clientAddress, &clientLen);
                if (newClient > 0) {
                    newFd.fd = newClient;
                    newFd.events = POLLIN;
                    newFd.revents = 0;
                    _fds.push_back(newFd);
                    _nbFds++;
                    _nbClients++;
                    _clientBuffers[newClient] = "";
                }
            }
            else {
                bytesRead = read(_fds[i].fd, buffer, sizeof(buffer) - 1);
                if (bytesRead <= 0) {
                    int fd = _fds[i].fd;
                    auto it = std::find_if(_users.begin(), _users.end(), [&fd](const User& u) {
                        return u.getFd() == fd;
                    });
                    if (it != _users.end()){
                        char uuidItStr[37];
                        uuid_unparse(it->getUuid().uuid, uuidItStr);
                        server_event_user_logged_out(uuidItStr);
                        std::string msg = "EVENT_LOGGED_OUT \"" + std::string(uuidItStr) + "\" \"" + it->getUsername() + "\"\n";
                        for (const auto& u : _users) {
                            if (u.isLoggedIn() && u.getFd() != -1 && u.getFd() != fd) {
                                write(u.getFd(), msg.c_str(), msg.length());
                            }
                        }
                        it->setFD(-1);
                        it->setLogState(false);
                    }
                    _clientBuffers.erase(fd);
                    close(_fds[i].fd);
                    _fds.erase(_fds.begin() + i);
                    _nbFds--;
                    _nbClients--;
                    i--;
                    continue;
                }
                buffer[bytesRead] = '\0';
                _clientBuffers[_fds[i].fd] += buffer;
                size_t pos;
                while ((pos = _clientBuffers[_fds[i].fd].find('\n')) != std::string::npos) {
                    std::string fullCommand = _clientBuffers[_fds[i].fd].substr(0, pos + 1);
                    _clientBuffers[_fds[i].fd].erase(0, pos + 1);
                    parser.parseCommands(fullCommand);
                    if (!parser.getCommand().empty())
                        handleCommand(parser, _fds[i].fd);
                }
            }
        }
    }
    return;
}
