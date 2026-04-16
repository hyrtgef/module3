#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int max_len = 0;
    char *max_str = "";
    
    for (int i = 1; i < argc; i++) {
        int len = strlen(argv[i]);
        if (len > max_len) {
            max_len = len;
            max_str = argv[i];
        }
    }
    
    printf("Максимальная длина: %d (\"%s\")\n", max_len, max_str);
    return 0;
}