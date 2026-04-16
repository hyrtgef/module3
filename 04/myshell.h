#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_CMD_LINE 1024
#define MAX_ARGS 64
#define MAX_CMDS 32   

int parse_pipeline(char *input, char *commands[], int max_cmds);

int parse_command(char *cmd, char *args[], int *in_fd, int *out_fd, char *in_file, char *out_file);

int execute_pipeline(char *commands[], int cmd_count);

#endif