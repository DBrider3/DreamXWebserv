CC = clang++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address -g3

RM = rm -rf

NAME = DreamX

SRC =	main.cpp\
		Manager.cpp\
		HttpBlock.cpp\
		ServerBlock.cpp\
		LocationBlock.cpp\
		ComposeServer.cpp\
		RunServer.cpp\
		Utils.cpp\
		ProcessMethod.cpp


SRC_DIR = ./src/
SRCS = $(addprefix $(SRC_DIR),$(SRC))

OBJ = $(SRC:.cpp=.o)
OBJ_DIR = ./obj/
OBJS = $(addprefix $(OBJ_DIR), $(OBJ))

INC = ./includes/

all : $(NAME)

$(NAME) : $(OBJS)
		$(CC) $(CPPFLAGS) -I $(INC) $^ \
		-o $@

$(OBJ_DIR)%.o : $(SRC_DIR)%.cpp
		@mkdir -p $(OBJ_DIR)
		$(CC) $(CPPFLAGS) -o $@ -c $<

clean:
		$(RM) $(OBJ_DIR)

fclean: clean
		$(RM) $(NAME)

re : fclean all

.PHONY : all clean fclean re
