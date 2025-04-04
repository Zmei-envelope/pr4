# Компилятор
CC = gcc
# Флаги компиляции
CFLAGS = -Wall -Wextra -I./src

# Директории
SRC_DIR = src
BUILD_DIR = build

# Исходные файлы
CLIENT_SRC = $(SRC_DIR)/client.c
SERVER_SRC = $(SRC_DIR)/server.c

# Исполняемые файлы (с указанием пути в build/)
CLIENT_EXE = $(BUILD_DIR)/client
SERVER_EXE = $(BUILD_DIR)/server

# Создаем папку build, если её нет
$(shell mkdir -p $(BUILD_DIR))

all: $(CLIENT_EXE) $(SERVER_EXE)

$(CLIENT_EXE): $(CLIENT_SRC)
	$(CC) $(CFLAGS) $< -o $@

$(SERVER_EXE): $(SERVER_SRC)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
