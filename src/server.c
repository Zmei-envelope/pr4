#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define MAX_BUFFER_SIZE 1024  // Максимальный размер буфера для сообщений

// Функция для логирования сообщений с IP-адресом клиента
void log_message(const char *client_ip, const char *message) {
    printf("%s: %s\n", client_ip, message);
}

int main(int argc, char *argv[]) {
    // Проверка аргументов командной строки
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <порт>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int server_fd, new_socket;  // Дескрипторы сокетов
    struct sockaddr_in address;  // Структура для адреса сервера
    int opt = 1;  // Опция для setsockopt
    int addrlen = sizeof(address);  // Размер структуры адреса
    char buffer[MAX_BUFFER_SIZE] = {0};  // Буфер для данных
    int port = atoi(argv[1]);  // Порт из аргументов

    // Создание TCP сокета
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Ошибка создания сокета");
        return EXIT_FAILURE;
    }

    // Настройка параметров сокета
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("Ошибка setsockopt");
        return EXIT_FAILURE;
    }

    // Настройка структуры адреса
    address.sin_family = AF_INET;  // IPv4
    address.sin_addr.s_addr = INADDR_ANY;  // Принимаем соединения на все интерфейсы
    address.sin_port = htons(port);  // Порт в сетевом порядке байтов

    // Привязка сокета к адресу и порту
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Ошибка привязки");
        return EXIT_FAILURE;
    }

    // Переводим сокет в режим ожидания соединений (максимум 3 в очереди)
    if (listen(server_fd, 3) < 0) {
        perror("Ошибка listen");
        return EXIT_FAILURE;
    }

    printf("Сервер запущен на порту %d\n", port);

    // Генерация случайного числа от 1 до 100
    srand(time(NULL));
    int number_to_guess = rand() % 100 + 1;
    printf("Загаданное число: %d\n", number_to_guess);

    // Принимаем входящее соединение
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Ошибка accept");
        return EXIT_FAILURE;
    }

    // Получаем IP-адрес клиента
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &address.sin_addr, client_ip, INET_ADDRSTRLEN);
    log_message(client_ip, "Подключен");

    int guessed_number;  // Число, которое угадывает клиент
    char message[MAX_BUFFER_SIZE];  // Сообщение для клиента
    int game_over = 0;  // Флаг завершения игры

    // Основной игровой цикл
    while (!game_over) {
        // Чтение числа от клиента
        int valread = read(new_socket, buffer, MAX_BUFFER_SIZE);
        if (valread <= 0) {
            log_message(client_ip, "Отключен");
            break;
        }

        guessed_number = atoi(buffer);  // Преобразуем строку в число

        // Проверяем угаданное число
        if (guessed_number < number_to_guess) {
            strcpy(message, "Больше");
        } else if (guessed_number > number_to_guess) {
            strcpy(message, "Меньше");
        } else {
            strcpy(message, "Правильно");
            game_over = 1;  // Игра завершена
        }

        // Отправляем ответ клиенту
        send(new_socket, message, strlen(message), 0);
        log_message(client_ip, message);

        if (game_over) {
            log_message(client_ip, "Игра завершена. Сервер отключается.");
        }
    }

    // Закрываем соединения
    close(new_socket);
    close(server_fd);
    
    return EXIT_SUCCESS;
}
