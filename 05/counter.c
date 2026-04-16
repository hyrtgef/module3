#define _POSIX_C_SOURCE 200809L
#include "counter.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

// Определение глобальных переменных
volatile sig_atomic_t keep_running = 1;
volatile sig_atomic_t sigint_count = 0;
FILE *output_file = NULL;

// Единая функция-обработчик сигналов
void signal_handler(int signum) {
    // Записываем информацию о полученном сигнале
    fprintf(output_file, "Получен и обработан сигнал: %d (%s)\n", 
            signum, strsignal(signum));
    fflush(output_file);
    
    // Особый случай для SIGINT - считаем количество
    if (signum == SIGINT) {
        sigint_count++;
        if (sigint_count >= 3) {
            fprintf(output_file, "Получен третий SIGINT. Завершение работы.\n");
            fflush(output_file);
            keep_running = 0;
        }
    }
}

// Функция инициализации программы
int init_program(void) {
    // Открываем файл для записи
    output_file = fopen("output.txt", "w");
    if (output_file == NULL) {
        perror("Ошибка открытия файла");
        return 0;
    }
    
    // Отключаем буферизацию
    setbuf(output_file, NULL);
    
    // Записываем стартовую информацию
    fprintf(output_file, "Программа запущена. PID: %d\n", getpid());
    
    // Выводим информацию в консоль
    printf("Программа запущена. PID: %d\n", getpid());
    printf("Для тестирования используйте:\n");
    printf("  kill -INT %d   (SIGINT, три раза для выхода)\n", getpid());
    printf("  kill -QUIT %d  (SIGQUIT, запись в лог)\n", getpid());
    printf("Просмотр лога: tail -f output.txt\n\n");
    
    // Настраиваем обработчики сигналов
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
    
    return 1;
}

// Основной цикл программы
void run_main_loop(void) {
    int counter = 1;
    struct timespec sleep_time = {1, 0};
    struct timespec remaining;
    
    while (keep_running) {
        // Записываем счетчик
        fprintf(output_file, "Счетчик: %d\n", counter);
        counter++;
        
        // Ждем 1 секунду
        nanosleep(&sleep_time, &remaining);
    }
}

// Функция завершения программы
void cleanup_program(void) {
    fprintf(output_file, "Программа завершена корректно.\n");
    printf("Программа завершена.\n");
    
    if (output_file != NULL) {
        fclose(output_file);
        output_file = NULL;
    }
}