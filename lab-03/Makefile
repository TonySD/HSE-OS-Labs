CC=gcc
CFLAGS=-O2 -Wall -Wextra -std=c11 -Iinclude
BUILD_DIR=build
SRC_DIR=src

OBJS=$(BUILD_DIR)/helpers.o

all: $(BUILD_DIR)/client $(BUILD_DIR)/server

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/helpers.o: $(SRC_DIR)/helpers.c include/helpers.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/helpers.c -o $(BUILD_DIR)/helpers.o

$(BUILD_DIR)/client: $(SRC_DIR)/client.c include/helpers.h $(BUILD_DIR)/helpers.o | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SRC_DIR)/client.c $(BUILD_DIR)/helpers.o -o $(BUILD_DIR)/client

$(BUILD_DIR)/server: $(SRC_DIR)/server.c include/helpers.h $(BUILD_DIR)/helpers.o | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SRC_DIR)/server.c $(BUILD_DIR)/helpers.o -o $(BUILD_DIR)/server

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean


