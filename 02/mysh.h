#ifndef MYSH_H
#define MYSH_H

#define MAX_INPUT 1024
#define MAX_ARGS 64

void print_prompt(void);
int parse_command(char *input, char *args[]);

void program_sum(int argc, char *argv[]);
void program_concat(int argc, char *argv[]);
void program_max(int argc, char *argv[]);

int is_builtin_test_program(char *cmd);
void run_builtin_test_program(char *cmd, int argc, char *argv[]);
void run_external_program(char *cmd, char *argv[]);

#endif