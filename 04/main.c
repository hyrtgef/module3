#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_CMD_LINE 1024
#define MAX_ARGS 64
#define MAX_CMDS 32

int parse_pipeline(char *input, char *commands[], int max_cmds) {
    int count = 0;
    char *token = strtok(input, "|");
    while (token != NULL && count < max_cmds) {
        while (*token == ' ') token++;
        char *end = token + strlen(token) - 1;
        while (end > token && *end == ' ') end--;
        *(end + 1) = '\0';
        
        commands[count] = strdup(token);
        if (!commands[count]) {
            perror("strdup");
            exit(EXIT_FAILURE);
        }
        count++;
        token = strtok(NULL, "|");
    }
    return count;
}

int parse_command(char *cmd, char *args[]) {
    int argc = 0;
    int i = 0;
    int len = strlen(cmd);
    
    while (i < len && argc < MAX_ARGS - 1) {
        while (i < len && cmd[i] == ' ') i++;
        if (i >= len) break;
        
        int start = i;
        
        while (i < len && cmd[i] != ' ') i++;
        
        int arg_len = i - start;
        args[argc] = malloc(arg_len + 1);
        strncpy(args[argc], cmd + start, arg_len);
        args[argc][arg_len] = '\0';
        argc++;
    }
    
    args[argc] = NULL;
    return 0;
}

void check_redirections(char *cmd, char **in_file, char **out_file) {
    char *pos;
    
    pos = strchr(cmd, '<');
    if (pos != NULL) {
        *pos = '\0';  
        pos++;
        while (*pos == ' ') pos++;
        char *end = pos;
        while (*end && *end != ' ' && *end != '>') end++;
        *in_file = strndup(pos, end - pos);
    }
    
    pos = strchr(cmd, '>');
    if (pos != NULL) {
        *pos = '\0';
        pos++;
        while (*pos == ' ') pos++;
        char *end = pos;
        while (*end && *end != ' ' && *end != '<') end++;
        *out_file = strndup(pos, end - pos);
    }
}

int execute_pipeline(char *commands[], int cmd_count) {
    int pipes[MAX_CMDS - 1][2];
    pid_t pids[MAX_CMDS];
    
    for (int i = 0; i < cmd_count - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            return -1;
        }
    }
    
    for (int i = 0; i < cmd_count; i++) {
        char *args[MAX_ARGS];
        char *in_file = NULL;
        char *out_file = NULL;
        
        char *cmd_copy = strdup(commands[i]);
        
        check_redirections(cmd_copy, &in_file, &out_file);
        
        parse_command(cmd_copy, args);
        
        if (args[0] == NULL) {
            fprintf(stderr, "Ошибка: пустая команда\n");
            free(cmd_copy);
            return -1;
        }
        
        pids[i] = fork();
        
        if (pids[i] == -1) {
            perror("fork");
            return -1;
        }
        
        if (pids[i] == 0) {
            if (i == 0 && in_file != NULL) {
                int fd = open(in_file, O_RDONLY);
                if (fd == -1) {
                    perror(in_file);
                    exit(EXIT_FAILURE);
                }
                dup2(fd, STDIN_FILENO);
                close(fd);
            } else if (i > 0) {
                dup2(pipes[i-1][0], STDIN_FILENO);
            }
            
            if (i == cmd_count - 1 && out_file != NULL) {
                int fd = open(out_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd == -1) {
                    perror(out_file);
                    exit(EXIT_FAILURE);
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            } else if (i < cmd_count - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }
            
            for (int j = 0; j < cmd_count - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            
            execvp(args[0], args);
            fprintf(stderr, "Команда не найдена: %s\n", args[0]);
            exit(EXIT_FAILURE);
        }
        
        free(cmd_copy);
        if (in_file) free(in_file);
        if (out_file) free(out_file);
        for (int j = 0; j < MAX_ARGS && args[j]; j++) free(args[j]);
    }
    
    for (int i = 0; i < cmd_count - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    
    for (int i = 0; i < cmd_count; i++) {
        waitpid(pids[i], NULL, 0);
    }
    
    return 0;
}

int main() {
    char input[MAX_CMD_LINE];
    char *commands[MAX_CMDS];
    
    while (1) {
        printf("myshell> ");
        fflush(stdout);
        
        if (!fgets(input, sizeof(input), stdin)) {
            printf("\n");
            break;
        }
        
        input[strcspn(input, "\n")] = '\0';
        
        if (strlen(input) == 0) {
            continue;
        }
        
        if (strcmp(input, "exit") == 0 || strcmp(input, "quit") == 0) {
            break;
        }
        
        int cmd_count = parse_pipeline(input, commands, MAX_CMDS);
        
        if (cmd_count == 0) {
            continue;
        }
        
        execute_pipeline(commands, cmd_count);
        
        for (int i = 0; i < cmd_count; i++) {
            free(commands[i]);
        }
    }
    
    return 0;
}