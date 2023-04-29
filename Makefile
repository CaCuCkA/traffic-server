CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99 -pedantic -g
LDFLAGS = -lpcap
OBJ_DIR = obj
SRC_DIR = src
INC_DIR = include

all: daemon

daemon: $(OBJ_DIR)/rbtree.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/rbtree.o: $(INC_DIR)/rbtree.h $(SRC_DIR)/rbtree.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) daemon

.PHONY: all clean