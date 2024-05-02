# Project: ircserv
NAME=ircserv

# Compiler options
CXX 		= c++
CXXFLAGS	= -g -Wall -Wextra -Werror -Wconversion -std=c++98
RM			= rm -rf
PRINT_INFO	= -info

# Colors
RED    		= "\033[0;31m"
GREEN  		= "\033[0;32m"
ORANGE 		= "\033[0;33m"
BLUE   		= "\033[0;34m"
RESET  		= "\033[0m"

# Folders
OBJ_FOLDER	   = ./obj/
SRC_FOLDER     = ./src/
INCLUDE_FOLDER = ./includes/
CXXINCLUDES    = -I$(INCLUDE_FOLDER)

# Files
SRCS 		= $(addprefix $(SRC_FOLDER), \
				main.cpp 	\
				Server.cpp 	\
				Channel.cpp \
				Client.cpp	\
				utils.cpp)

# Includes
INC 		= $(addprefix $(INCLUDE_FOLDER), \
				Server.hpp	\
				Channel.hpp	\
				Client.hpp	\
				utils.hpp)

# Object files
OBJS 		= $(SRCS:%.cpp=$(OBJ_FOLDER)%.o)

# Targets
.PHONY: all clean fclean re MSG_START MSG_DONE run val lol sub runNoPort

all: MSG_START $(NAME) MSG_DONE

$(NAME): $(OBJS)
	@$(CXX) $(OBJS) $(CXXFLAGS) $(CXXINCLUDES) -o $(NAME)
	@echo $(GREEN)"\n Linking complete!"$(RESET)

$(OBJ_FOLDER)%.o: %.cpp
	@mkdir -p $(@D)
	@echo -n $(GREEN)"."$(RESET)
	@$(CXX) $(CXXFLAGS) $(CXXINCLUDES) -c $< -o $@

clean:
	@$(RM) $(OBJ_FOLDER)
	@echo $(RED) $(NAME) "removed object files" $(RESET)

fclean: clean
	@$(RM) $(NAME)
	@echo $(RED) $(NAME) "removed program" $(RESET)

re: fclean all

runNoPort: re
	@echo $(BLUE) $(NAME) "starting..." $(RESET)
	@./$(NAME) 808000000 42

run: re
	@echo $(BLUE) $(NAME) "starting..." $(RESET)
	@./$(NAME) 8080 42

val: re
	@echo $(BLUE) $(NAME) "starting with valgrind..." $(RESET)
	@valgrind --leak-check=full ./$(NAME) 8080 42

MSG_START:
	@echo $(ORANGE) $(NAME) "compiling" $(RESET)

MSG_DONE:
	@echo $(GREEN) $(NAME) "compiled" $(RESET)

sub:
	evince *pdf &

lol:
	curl parrot.live