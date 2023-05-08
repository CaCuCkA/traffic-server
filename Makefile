CC = gcc
CFLAGS = -Wall -Wextra -I$(INC_DIR)
LDFLAGS = -lpcap
OBJ_DIR = obj
SRC_DIR = src
INC_DIR = include

all: daemon

daemon: $(OBJ_DIR)/daemon.o $(OBJ_DIR)/server.o $(OBJ_DIR)/rbtree.o $(OBJ_DIR)/main.o
	$(CC) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/daemon.o: $(INC_DIR)/daemon.h $(SRC_DIR)/daemon.c $(INC_DIR)/server.h $(INC_DIR)/exception.h $(INC_DIR)/rbtree.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/daemon.c -o $@

$(OBJ_DIR)/server.o: $(INC_DIR)/server.h $(SRC_DIR)/server.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/server.c -o $@

$(OBJ_DIR)/rbtree.o: $(INC_DIR)/rbtree.h $(SRC_DIR)/rbtree.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/rbtree.c -o $@

$(OBJ_DIR)/main.o: $(SRC_DIR)/main.c $(INC_DIR)/server.h $(INC_DIR)/daemon.h $(INC_DIR)/exception.h $(INC_DIR)/rbtree.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/main.c -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) daemon

.PHONY: all clean
