#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define MAX_BUFFER_SIZE 1024

void log_message(const char *client_ip, const char *message) {
    printf("%s: %s\n", client_ip, message);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <порт>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[MAX_BUFFER_SIZE] = {0};
    int port = atoi(argv[1]);

    // Создание сокета
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Ошибка создания сокета");
        exit(EXIT_FAILURE);
    }

    // Настройка сокета
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("Ошибка setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Привязка сокета к порту
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Ошибка привязки");
        exit(EXIT_FAILURE);
    }

    // Ожидание подключений
    if (listen(server_fd, 3) < 0) {
        perror("Ошибка listen");
        exit(EXIT_FAILURE);
    }

    printf("Сервер запущен на порту %d\n", port);

    // Генерация случайного числа
    srand(time(NULL));
    int number_to_guess = rand() % 100 + 1;
    printf("Загаданное число: %d\n", number_to_guess); // Вывод загаданного числа

    while (1) {
        // Принятие нового соединения
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Ошибка accept");
            exit(EXIT_FAILURE);
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &address.sin_addr, client_ip, INET_ADDRSTRLEN);
        log_message(client_ip, "Подключен");

        int guessed_number = 0;
        char message[MAX_BUFFER_SIZE];

        while (1) {
            // Чтение данных от клиента
            int valread = read(new_socket, buffer, MAX_BUFFER_SIZE);
            if (valread <= 0) {
                log_message(client_ip, "Отключен");
                break;
            }

            guessed_number = atoi(buffer);
            if (guessed_number < number_to_guess) {
                strcpy(message, "Больше");
            } else if (guessed_number > number_to_guess) {
                strcpy(message, "Меньше");
            } else {
                strcpy(message, "Правильно");
            }

            // Отправка ответа клиенту
            send(new_socket, message, strlen(message), 0);
            log_message(client_ip, message);

            if (strcmp(message, "Правильно") == 0) {
                log_message(client_ip, "Игра завершена");
                break;
            }
        }

        close(new_socket);
    }

    return 0;
}
