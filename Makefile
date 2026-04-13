NAME = webserv
CPP = c++
RM = rm -rf

################################################################################
###############                  DIRECTORIES                      ##############
################################################################################

OBJ_DIR = _obj
INC_DIRS := inc
SRC_DIRS := src

vpath %.hpp $(INC_DIRS)
vpath %.cpp $(SRC_DIRS)

################################################################################
###############                  SOURCE FILES                     ##############
################################################################################

SOCKETS_FILES := body_creation.cpp connection_class.cpp event_handling.cpp event_loop.cpp socket_creation.cpp cgi_event_handling.cpp
SOCKETS := $(addprefix sockets/, $(SOCKETS_FILES))

SOCKETS_UTILS_FILES := utils.cpp event_loop_if_statments.cpp
SOCKETS_UTILS := $(addprefix sockets/utils/, $(SOCKETS_UTILS_FILES))

PARSER_FILES := parseConfig.cpp utils.cpp HttpRequest.cpp
PARSER := $(addprefix parsing/, $(PARSER_FILES))

RESPONSE_FILES := HttpResponse.cpp route_matching.cpp method_check.cpp build_path.cpp check_file.cpp error.cpp cgi.cpp
RESPONSE := $(addprefix response/, $(RESPONSE_FILES))

METHODS_FILES := GET.cpp POST.cpp DELETE.cpp
METHODS := $(addprefix methods/, $(METHODS_FILES))

SRC_FILES := main.cpp webserv.cpp
SRC := $(addprefix src/, $(SRC_FILES) $(PARSER) $(SOCKETS) $(SOCKETS_UTILS) $(RESPONSE) $(METHODS))

# Test files
TEST_NAME := test_response
TEST_SRC := src/response/test.cpp \
			src/response/HttpResponse.cpp \
			src/response/route_matching.cpp \
			src/response/method_check.cpp \
			src/response/build_path.cpp \
			src/response/check_file.cpp \
			src/response/error.cpp \
			src/methods/GET.cpp \
			src/methods/POST.cpp \
			src/methods/DELETE.cpp

################################################################################
###############                     RULES                         ##############
################################################################################

OBJS := $(addprefix $(OBJ_DIR)/, $(SRC:%.cpp=%.o))

# Compilation flags and linking options
CFLAGS := -Wall -Wextra -Wno-shadow -std=c++17 $(addprefix -I, $(INC_DIRS))

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

test: $(TEST_NAME)

$(TEST_NAME): $(TEST_SRC)
	@echo "$(COLOR_CYAN)Building test suite...$(COLOR_RESET)"
	@$(CPP) $(CFLAGS) $(TEST_SRC) -o $(TEST_NAME)
	@echo "$(COLOR_GREEN)Test suite built successfully!$(COLOR_RESET)"
	@echo "$(COLOR_CYAN)Run with: ./$(TEST_NAME)$(COLOR_RESET)"

$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	@echo "$(COLOR_GREEN)Compiling $<...$(COLOR_RESET)"
	@$(CPP) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

clean:
	@echo "$(COLOR_YELLOW)Cleaning object files...$(COLOR_RESET)"
	@$(RM) $(OBJ_DIR)

fclean: clean
	@echo "$(COLOR_YELLOW)Removing $(NAME)...$(COLOR_RESET)"
	@$(RM) $(NAME)
	@echo "$(COLOR_YELLOW)Removing $(TEST_NAME)...$(COLOR_RESET)"
	@$(RM) $(TEST_NAME)
	@echo "$(COLOR_YELLOW)All files removed successfully$(COLOR_RESET)"

re: fclean all

help:
	@echo "Available targets:"
	@echo "  all      - Build $(NAME) (default)"
	@echo "  test     - Build and compile test suite"
	@echo "  clean    - Remove object files"
	@echo "  fclean   - Remove all generated files"
	@echo "  re       - Rebuild $(NAME)"

-include $(OBJS:.o=.d)

kill:
	bash kill -9 $(lsof -t -i :8080)

.PHONY: all test clean fclean re help kill