NAME := webserv

CC := c++
CFLAGS := -std=c++17 -Wall -Wextra -Werror -g3 -fsanitize=address
INCLUDES := -I./includes -I/usr/local/include
LIBS := -L/usr/local/lib -lCatch2
NUM_CORES := $(shell nproc)
MAKEFLAGS += -j$(NUM_CORES)

SRC_DIR := srcs
OBJ_DIR := obj
TEST_DIR := tests

SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

TEST_SRCS := $(shell find $(TEST_DIR) -name '*.cpp')
TEST_OBJS := $(patsubst $(TEST_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(TEST_SRCS))

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

test: $(filter-out obj/main.o, $(OBJS)) $(TEST_OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $(filter-out obj/main.o, $(OBJS)) $(TEST_OBJS) -o $(TEST_NAME) $(LIBS)
	# ./$(TEST_NAME)

clean:
	rm -rf $(OBJ_DIR) $(TEST_OBJS)

fclean: clean
	rm -f $(NAME) $(TEST_NAME)

run: all
	./$(NAME) "test"

re: fclean all

.PHONY: clean fclean all re run
