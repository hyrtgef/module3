#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "processor.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <arg1> <arg2> ...\n", argv[0]);
        return 1;
    }
    
    int total = argc - 1;
    int half = total / 2;
    
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("Ошибка: fork");
        return 1;
    }
    
    if (pid == 0) {
        printf("\nДочерний процесс (PID: %d)\n", getpid());
        for (int i = 1 + half; i < argc; i++) {
            process_argument(argv[i]);
        }
        exit(0);
    } else {
        printf("Родительский процесс (PID: %d)\n", getpid());
        for (int i = 1; i <= half; i++) {
            process_argument(argv[i]);
        }
        
        wait(NULL);
        printf("\nОба процесса завершили работу.\n");
    }
    
    return 0;
}