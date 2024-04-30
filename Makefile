NAME=ircserv

# Compiler options
CC 			= c++
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
OBJ_FOLDER	= ./obj/

# Files
SRCS 		= main.cpp

# Object files
OBJS 		= $(SRCS:%.cpp=$(OBJ_FOLDER)%.o)

# Targets
.PHONY: all clean fclean re MSG_START MSG_DONE run val lol sub

all: MSG_START $(NAME) MSG_DONE

$(NAME): $(OBJS)
	@$(CC) $(OBJS) $(CXXFLAGS) -o $(NAME)
	@echo $(GREEN)"\n Linking complete!"$(RESET)

$(OBJ_FOLDER)%.o: %.cpp
	@mkdir -p $(@D)
	@echo -n $(GREEN)"."$(RESET)
	@$(CC) $(CXXFLAGS) -c $< -o $@

clean:
	@$(RM) $(OBJ_FOLDER)
	@echo $(RED) $(NAME) "removed object files" $(RESET)

fclean: clean
	@$(RM) $(NAME)
	@echo $(RED) $(NAME) "removed program" $(RESET)

re: fclean all

run: re
	@echo $(BLUE) $(NAME) "starting..." $(RESET)
	@echo $(BLUE) $(NAME) "TO DO..." $(RESET)
#	@./$(NAME) $(TEST_FILE)

val: re
	@echo $(BLUE) $(NAME) "starting with valgrind..." $(RESET)
	@echo $(BLUE) $(NAME) "TO DO..." $(RESET)
#	@valgrind --leak-check=full ./$(NAME) $(TEST_FILE) $(PRINT_INFO)

MSG_START:
	@echo $(ORANGE) $(NAME) "compiling" $(RESET)

MSG_DONE:
	@echo $(GREEN) $(NAME) "compiled" $(RESET)

sub:
	evince *pdf &

lol:
	curl parrot.live