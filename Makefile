NAME = webserv
CPP = c++
RM = rm -rf
UNAME_S := $(shell uname -s)

################################################################################
###############                  DIRECTORIES                      ##############
################################################################################

OBJ_DIR = _obj
INC_DIRS := Includes
SRC_DIRS := src

vpath %.hpp $(INC_DIRS)
vpath %.cpp $(SRC_DIRS)

################################################################################
###############                  SOURCE FILES                     ##############
################################################################################

EXECUTION_FILES := execution.cpp
EXECUTION := $(addprefix execution/, $(EXECUTION_FILES))

PARSER_FILES := parser.cpp
PARSER := $(addprefix parser/, $(PARSER_FILES))

SRC_FILES := main.cpp
SRC := $(addprefix src/, $(SRC_FILES) $(EXECUTION) $(PARSER))

################################################################################
###############               OBJECT FILES & RULES                ##############
################################################################################

OBJS := $(addprefix $(OBJ_DIR)/, $(SRC:%.cpp=%.o))

# Compilation flags and linking options
CFLAGS := -Wall -Wextra -Werror -Wpedantic -Wshadow -std=c++17 -Wconversion -Wsign-conversion -g -MMD -MP $(addprefix -I, $(INC_DIRS))
LDFLAGS_SAN :=

# Color definitions for better terminal output
COLOR_RESET = \033[0m
COLOR_GREEN = \033[0;32m
COLOR_BLUE = \033[0;34m
COLOR_CYAN = \033[0;36m
COLOR_YELLOW = \033[0;33m

all: $(NAME)

$(NAME): $(OBJS)
	@echo "$(COLOR_GREEN)Creating $(NAME)...$(COLOR_RESET)"
	@$(CPP) $(LDFLAGS) $(OBJS) -o $(NAME)
	@echo "$(COLOR_GREEN)Successful Compilation of $(NAME)$(COLOR_RESET)"

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo "$(COLOR_GREEN)Compiling $<...$(COLOR_RESET)"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "$(COLOR_YELLOW)Cleaning object files...$(COLOR_RESET)"
	@$(RM) $(OBJ_DIR)

fclean: clean
	@echo "$(COLOR_YELLOW)Removing $(NAME)...$(COLOR_RESET)"
	@$(RM) $(NAME)
	@echo "$(COLOR_YELLOW)All files removed successfully$(COLOR_RESET)"

re: fclean all

help:
	@echo "Available targets:"
	@echo "  all      - Build $(NAME) (default)"
	@echo "  clean    - Remove object files"
	@echo "  fclean   - Remove all generated files"
	@echo "  re       - Rebuild $(NAME)"

-include $(OBJS:.o=.d)

.PHONY: all clean fclean re help