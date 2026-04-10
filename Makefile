##
## EPITECH PROJECT, 2024
## makefile
## File description:
## makefile
##

NAME    =   myteams_server

CXX     =   clang++

## Main
SRC     =   src/main.cpp

## User
SRC		+=	src/user/User.cpp

## Server
SRC		+=	src/server/Server.cpp

## Parser
SRC		+=	src/parser/Parser.cpp

OBJ     =   $(SRC:.cpp=.o)

CXXFLAGS  =   -I./src/ -I./libs/myteams -Wall -Wextra

LDFLAGS   = -L./libs/myteams -lmyteams -Wl,-rpath=./libs/myteams -luuid

all:    $(NAME)

$(NAME):    $(OBJ)
	$(CXX) -o $(NAME) $(OBJ) $(LDFLAGS)

%.o:    %.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re:     fclean all

debug: CXXFLAGS += -g
debug: re

.PHONY: all clean fclean re debug