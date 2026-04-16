#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include "mysh.h"

void print_prompt() {
    printf("mysh> ");
    fflush(stdout);
}

int parse_command(char *input, char *args[]) {
    char *token;
    int count = 0;
    
    token = strtok(input, " \t\n");
    while (token != NULL && count < MAX_ARGS - 1) {
        args[count++] = token;
        token = strtok(NULL, " \t\n");
    }
    args[count] = NULL;
    
    return count;
}

void program_sum(int argc, char *argv[]) {
    if (argc == 1) {
        printf("Сумма: 0\n");
        return;
    }
    
    int sum = 0;
    for (int i = 1; i < argc; i++) {
        sum += atoi(argv[i]);
    }
    printf("Сумма: %d\n", sum);
}

void program_concat(int argc, char *argv[]) {
    if (argc == 1) {
        printf("\n");
        return;
    }
    
    char result[MAX_INPUT] = "";
    for (int i = 1; i < argc; i++) {
        strcat(result, argv[i]);
        if (i < argc - 1) {
            strcat(result, " ");
        }
    }
    printf("%s\n", result);
}

void program_max(int argc, char *argv[]) {
    if (argc == 1) {
        printf("Ошибка: нет аргументов\n");
        return;
    }
    
    int is_all_numbers = 1;
    for (int i = 1; i < argc; i++) {
        for (int j = 0; j < strlen(argv[i]); j++) {
            if (!isdigit(argv[i][j]) && argv[i][j] != '-') {
                is_all_numbers = 0;
                break;
            }
        }
    }
    
    if (is_all_numbers) {
        int max_val = atoi(argv[1]);
        for (int i = 2; i < argc; i++) {
            int val = atoi(argv[i]);
            if (val > max_val) max_val = val;
        }
        printf("Максимальное число: %d\n", max_val);
    } else {
        int max_len = strlen(argv[1]);
        char *max_str = argv[1];
        for (int i = 2; i < argc; i++) {
            int len = strlen(argv[i]);
            if (len > max_len) {
                max_len = len;
                max_str = argv[i];
            }
        }
        printf("Строка с максимальной длиной: \"%s\" (длина: %d)\n", max_str, max_len);
    }
}

int is_builtin_test_program(char *cmd) {
    return (strcmp(cmd, "sum") == 0 ||
            strcmp(cmd, "concat") == 0 ||
            strcmp(cmd, "max") == 0);
}

void run_builtin_test_program(char *cmd, int argc, char *argv[]) {
    if (strcmp(cmd, "sum") == 0) {
        program_sum(argc, argv);
    } else if (strcmp(cmd, "concat") == 0) {
        program_concat(argc, argv);
    } else if (strcmp(cmd, "max") == 0) {
        program_max(argc, argv);
    }
}

void run_external_program(char *cmd, char *argv[]) {
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("Ошибка при создании процесса");
        return;
    }
    
    if (pid == 0) {
        execvp(cmd, argv);
        
        printf("Ошибка: программа '%s' не найдена\n", cmd);
        exit(1);
    } else {
        int status;
        waitpid(pid, &status, 0);
    }
}