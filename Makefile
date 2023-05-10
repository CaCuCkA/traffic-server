CC = gcc
CFLAGS = -Wall -Wextra -I$(INC_DIR)
LDFLAGS = -lpcap
OBJ_DIR = obj
BIN_DIR = bin
SRC_DIR = src
INC_DIR = include
DEFINES = -DDAEMON

all: $(BIN_DIR)/daemon $(BIN_DIR)/client

$(BIN_DIR)/client: $(OBJ_DIR)/client.o $(OBJ_DIR)/client_server.o $(OBJ_DIR)/client_main.o $(OBJ_DIR)/client_utils.o | $(BIN_DIR)
	$(CC) $^ -o $@ $(LDFLAGS)

$(BIN_DIR)/daemon: $(OBJ_DIR)/daemon.o $(OBJ_DIR)/daemon_server.o $(OBJ_DIR)/rbtree.o $(OBJ_DIR)/daemon_main.o $(OBJ_DIR)/daemon_utils.o | $(BIN_DIR)
	$(CC) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/daemon.o: $(INC_DIR)/daemon.h $(SRC_DIR)/daemon.c $(INC_DIR)/server.h $(INC_DIR)/exception.h $(INC_DIR)/rbtree.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(DEFINES) -c $(SRC_DIR)/daemon.c -o $@

$(OBJ_DIR)/client.o: $(INC_DIR)/client.h $(SRC_DIR)/client.c $(INC_DIR)/server.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/client.c -o $@

$(OBJ_DIR)/daemon_server.o: $(INC_DIR)/server.h $(SRC_DIR)/server.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(DEFINES) -c $(SRC_DIR)/server.c -o $@

$(OBJ_DIR)/daemon_utils.o: $(INC_DIR)/utils.h $(SRC_DIR)/utils.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(DEFINES) -c $(SRC_DIR)/utils.c -o $@

$(OBJ_DIR)/client_utils.o: $(INC_DIR)/utils.h $(SRC_DIR)/utils.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/utils.c -o $@

$(OBJ_DIR)/client_server.o: $(INC_DIR)/server.h $(SRC_DIR)/server.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/server.c -o $@

$(OBJ_DIR)/rbtree.o: $(INC_DIR)/rbtree.h $(SRC_DIR)/rbtree.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(DEFINES) -c $(SRC_DIR)/rbtree.c -o $@

$(OBJ_DIR)/daemon_main.o: $(SRC_DIR)/main.c $(INC_DIR)/daemon.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(DEFINES) -c $(SRC_DIR)/main.c -o $@

$(OBJ_DIR)/client_main.o: $(SRC_DIR)/main.c $(INC_DIR)/client.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/main.c -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(OBJ_DIR) daemon client

.PHONY: all clean
