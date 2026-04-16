#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    char line[1024];
    int sum = 0;
    while(fgets(line,sizeof(line),stdin))
    {
        char *token = strtok(line, " \t\n");
        while(token)
        {
            sum += atoi(token);
            token = strtok(NULL, " \t\n");
        }
    }
    
    printf("Сумма: %d\n", sum);
    return 0;
}