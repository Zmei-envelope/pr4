#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Использование: %s <адрес сервера> <порт>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[MAX_BUFFER_SIZE] = {0};
    char *server_ip = argv[1];
    int port = atoi(argv[2]);

    // Создание сокета
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Ошибка создания сокета");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Преобразование IP-адреса из текстового в бинарный формат
    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        perror("Ошибка преобразования адреса");
        exit(EXIT_FAILURE);
    }

    // Подключение к серверу
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Ошибка подключения");
        exit(EXIT_FAILURE);
    }

    printf("Подключено к серверу %s:%d\n", server_ip, port);

    while (1) {
        printf("Введите число: ");
        fgets(buffer, MAX_BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0; // Удаление символа новой строки

        // Отправка числа серверу
        send(sock, buffer, strlen(buffer), 0);

        // Получение ответа от сервера
        int valread = read(sock, buffer, MAX_BUFFER_SIZE);
        if (valread <= 0) {
            printf("Сервер отключился\n");
            break;
        }

        buffer[valread] = '\0';
        printf("Ответ сервера: %s\n", buffer);

        if (strcmp(buffer, "Правильно") == 0) {
            printf("Вы угадали число!\n");
            break;
        }
    }

    close(sock);
    return 0;
}
