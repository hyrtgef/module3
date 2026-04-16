#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mysh.h"

int main() {
    char input[MAX_INPUT];
    char *args[MAX_ARGS];
    int argc;
    
    printf("Простой командный интерпретатор\n");
    printf("Доступные команды:\n");
    printf("  exit, quit - выход из интерпретатора\n");
    printf("  help - справка\n");
    printf("  sum <числа> - сумма чисел\n");
    printf("  concat <строки> - склеивание строк\n");
    printf("  max <аргументы> - максимум чисел или строка с макс. длиной\n");
    printf("  Также можно запускать любые системные программы\n");
    
    while (1) {
        print_prompt();
        
        if (fgets(input, MAX_INPUT, stdin) == NULL) {
            printf("\n");
            break;
        }
        
        input[strcspn(input, "\n")] = 0;
        
        if (strlen(input) == 0) {
            continue;
        }
        
        argc = parse_command(input, args);
        if (argc == 0) continue;
        
        if (strcmp(args[0], "exit") == 0 || strcmp(args[0], "quit") == 0) {
            printf("Выход из интерпретатора...\n");
            break;
        }
        
        if (strcmp(args[0], "help") == 0) {
            printf("Доступные команды:\n");
            printf("  sum <числа> - вычисляет сумму чисел\n");
            printf("  concat <строки> - склеивает строки через пробел\n");
            printf("  max <аргументы> - если все аргументы числа, выводит максимум,\n");
            printf("                    иначе выводит строку с максимальной длиной\n");
            printf("  exit, quit - выход из интерпретатора\n");
            printf("  help - эта справка\n");
            printf("  Также можно запускать любые системные программы (ls, ps, echo и т.д.)\n");
            continue;
        }
        
        if (is_builtin_test_program(args[0])) {
            run_builtin_test_program(args[0], argc, args);
        } else {
            run_external_program(args[0], args);
        }
    }
    
    return 0;
}