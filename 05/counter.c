#define _POSIX_C_SOURCE 200809L
#include "counter.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

volatile sig_atomic_t keep_running = 1;
volatile sig_atomic_t sigint_count = 0;
FILE *output_file = NULL;

void signal_handler(int signum) {
    fprintf(output_file, "Получен и обработан сигнал: %d (%s)\n", 
            signum, strsignal(signum));
    fflush(output_file);
    
    if (signum == SIGINT) {
        sigint_count++;
        if (sigint_count >= 3) {
            fprintf(output_file, "Получен третий SIGINT. Завершение работы.\n");
            fflush(output_file);
            keep_running = 0;
        }
    }
}


int init_program(void) {
    output_file = fopen("output.txt", "w");
    if (output_file == NULL) {
        perror("Ошибка открытия файла");
        return 0;
    }
    
    setbuf(output_file, NULL);
    
    fprintf(output_file, "Программа запущена. PID: %d\n", getpid());
    
    printf("Программа запущена. PID: %d\n", getpid());
    printf("Для тестирования используйте:\n");
    printf("  kill -INT %d   (SIGINT, три раза для выхода)\n", getpid());
    printf("  kill -QUIT %d  (SIGQUIT, запись в лог)\n", getpid());
    printf("Просмотр лога: tail -f output.txt\n\n");
    
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
    
    return 1;
}

void run_main_loop(void) {
    int counter = 1;
    struct timespec sleep_time = {1, 0};
    struct timespec remaining;
    
    while (keep_running) {
        fprintf(output_file, "Счетчик: %d\n", counter);
        counter++;
        
        nanosleep(&sleep_time, &remaining);
    }
}

void cleanup_program(void) {
    fprintf(output_file, "Программа завершена корректно.\n");
    printf("Программа завершена.\n");
    
    if (output_file != NULL) {
        fclose(output_file);
        output_file = NULL;
    }
}