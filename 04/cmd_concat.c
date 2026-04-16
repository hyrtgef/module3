#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    char result[1024] = "";
    for (int i = 1; i < argc; i++) {
        strcat(result, argv[i]);
        if (i < argc - 1) strcat(result, " ");
    }
    printf("%s\n", result);
    return 0;
}