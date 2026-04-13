#include "client/Client.hpp"

Client::Client::Client(std::string adress, std::string port)
{
    _adress = adress;
    _port = port;
}

void Client::Client::handleEvent(std::string fullMessage, Parser &parser)
{
    parser.parseCommands(fullMessage);
    std::string command = parser.getCommand();
    std::vector<std::string> arguments = parser.getArgs();

    if (command == "EVENT_LOGGED_IN" && arguments.size() == 2){
        client_event_logged_in(arguments[0].c_str(), arguments[1].c_str());
    }
    else if (command == "EVENT_LOGGED_OUT" && arguments.size() == 2){
        client_event_logged_out(arguments[0].c_str(), arguments[1].c_str());
    }
    else if (command == "EVENT_USERS" && arguments.size() == 3){
        int userStatus = std::stoi(arguments[2].c_str());
        client_print_users(arguments[0].c_str(), arguments[1].c_str(), userStatus);
    }
    else if (command == "EVENT_USER" && arguments.size() == 3){
        int userStatus = std::stoi(arguments[2].c_str());
        client_print_user(arguments[0].c_str(), arguments[1].c_str(), userStatus);
    }
    else if (command == "EVENT_USER_DON'T_EXISTS" && arguments.size() == 1){
        client_error_unknown_user(arguments[0].c_str());   
    }
    else if (command == "EVENT_MESSAGE_SENT" && arguments.size() == 2){
        client_event_private_message_received(arguments[0].c_str(), arguments[1].c_str());
    }
    else if (command == "EVENT_MESSAGE_LIST" && arguments.size() == 3){
        client_private_message_print_messages(arguments[0].c_str(), static_cast<time_t>(std::stoll(arguments[1])), arguments[2].c_str());
    }
    else if (command == "EVENT_UNKNOWN_TEAM" && arguments.size() == 1){
        client_error_unknown_team(arguments[0].c_str());
    }
    else if (command == "EVENT_UNKNOWN_CHANNEL" && arguments.size() == 1){
        client_error_unknown_channel(arguments[0].c_str());
    }
    else if (command == "EVENT_UNKNOWN_THREAD" && arguments.size() == 1){
        client_error_unknown_thread(arguments[0].c_str());
    }
    else if (command == "PERSONAL_TEAM_CREATED" && arguments.size() == 3){
        client_print_team_created(arguments[0].c_str(), arguments[1].c_str(), arguments[2].c_str());
    }
    else if (command == "EVENT_TEAM_CREATED" && arguments.size() == 3){
        client_event_team_created(arguments[0].c_str(), arguments[1].c_str(), arguments[2].c_str());
    }
    else if (command == "EVENT_ALREADY_EXIST" && arguments.size() == 0){
        client_error_already_exist();
    }
    else return;
}

void Client::Client::runClient()
{
    int clientFD = socket(AF_INET, SOCK_STREAM, 0);
    struct pollfd stdinFD;
    char buffer[4096];
    ssize_t bytesRead;
    struct pollfd serverFD;
    Parser parser;

    if (clientFD < 0)
        throw std::runtime_error("Failed to create client socket");
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(std::stoi(_port));
    if (inet_pton(AF_INET, _adress.c_str(), &server_addr.sin_addr) <= 0)
        throw std::runtime_error("IP adress is invalid.");
    if (connect(clientFD, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        throw std::runtime_error("Failed to connect to the server");
    stdinFD.fd = STDIN_FILENO;
    stdinFD.events = POLLIN;
    stdinFD.revents = 0;
    _fds.push_back(stdinFD);
    serverFD.fd = clientFD;
    serverFD.events = POLLIN;
    serverFD.revents = 0;
    _fds.push_back(serverFD);
    std::string serverBuffer = "";

    while (1) {
        if (poll(_fds.data(), _fds.size(), NO_TIMEOUT) == -1)
            break;
        if (_fds[0].revents & POLLIN) {
            std::string input;
            if (!getline(std::cin, input))
                break;
            parser.parseCommands(input);
            std::string toSend = input + '\n';
            write(clientFD, toSend.c_str(), strlen(toSend.c_str()));
        }
        if (_fds[1].revents & POLLIN) {
            bytesRead = read(clientFD, buffer, sizeof(buffer) - 1);
            if (bytesRead <= 0){
                break;
            }
            buffer[bytesRead] = '\0';
            serverBuffer += buffer;
            size_t pos;
            while ((pos = serverBuffer.find('\n')) != std::string::npos) {
                std::string fullMessage = serverBuffer.substr(0, pos + 1);
                serverBuffer.erase(0, pos + 1);
                handleEvent(fullMessage, parser);
            }
        }
    }
    close(clientFD);
}