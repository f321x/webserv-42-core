NAME := webserv

CC := c++
CFLAGS := -std=c++17 -Wall -Wextra -Werror
INCLUDES := -I./includes -I/usr/local/include
LIBS := -L/usr/local/lib -lCatch2

SRC_DIR := srcs
OBJ_DIR := obj
TEST_DIR := tests

SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

TEST := $(shell find $(TEST_DIR) -name '*.cpp')
TEST_OBJS := $(patsubst $(TEST_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(TEST))

CATCH2_MAIN := $(OBJ_DIR)/test_config.cpp

TEST_NAME := test

all: $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# compiling objects for tests
$(OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $(NAME)

test: $(OBJS) $(TEST_OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) $(TEST_OBJS) -o $(TEST_NAME) $(LIBS)
	./$(TEST_NAME)

clean:
	rm -rf $(OBJ_DIR) $(TEST_OBJS)

fclean: clean
	rm -f $(NAME) $(TEST_NAME)

run: all
	./$(NAME)

re: fclean all

.PHONY: clean fclean all re run
