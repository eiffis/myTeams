##
## EPITECH PROJECT, 2024
## makefile
## File description:
## makefile
##

SERVER    =   myteams_server
CLIENT    =   myteams_cli

CXX     =   clang++

## SERVER_SRC
SERVER_SRC		=   src/main.cpp
SERVER_SRC      +=  src/server/Server.cpp
SERVER_SRC      +=  src/user/User.cpp
SERVER_SRC		+= 	src/thread/Thread.cpp
SERVER_SRC		+=  src/team/Team.cpp
SERVER_SRC		+=  src/channel/Channel.cpp

## CLIENT_SRC 
CLIENT_SRC  =  src/client/clientMain.cpp
CLIENT_SRC  += src/client/Client.cpp

## Parser
COMMON_SRC  = src/parser/Parser.cpp

SERVER_OBJ  = $(SERVER_SRC:.cpp=.o) $(COMMON_SRC:.cpp=.o)
CLIENT_OBJ  = $(CLIENT_SRC:.cpp=.o) $(COMMON_SRC:.cpp=.o)

CXXFLAGS  =   -I./src/ -I./libs/myteams -Wall -Wextra

LDFLAGS   = -L./libs/myteams -lmyteams -Wl,-rpath=./libs/myteams -luuid

all:    $(SERVER) $(CLIENT)

$(SERVER): $(SERVER_OBJ)
	$(CXX) -o $(SERVER) $(SERVER_OBJ) $(LDFLAGS)

$(CLIENT): $(CLIENT_OBJ)
	$(CXX) -o $(CLIENT) $(CLIENT_OBJ) $(LDFLAGS)

%.o:    %.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

clean:
	rm -f $(SERVER_OBJ) $(CLIENT_OBJ)

fclean: clean
	rm -f $(SERVER) $(CLIENT)

re:     fclean all

debug: CXXFLAGS += -g
debug: re

.PHONY: all clean fclean re debug