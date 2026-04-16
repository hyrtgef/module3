#include "processor.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

int is_number(const char *str) {
    int i = 0;
    int has_digit = 0;
    int has_dot = 0;
    
    if (str[0] == '-') i++;
    
    for (; str[i] != '\0'; i++) {
        if (isdigit(str[i])) {
            has_digit = 1;
        } else if (str[i] == '.') {
            if (has_dot) return 0; 
            has_dot = 1;
        } else {
            return 0;
        }
    }
    
    return has_digit;
}

void process_argument(const char *arg) {
    if (is_number(arg)) {
       
        double value = atof(arg);
        printf("%s -> %.6f, %.6f\n", arg, value, value * 2);
    } else {
        printf("%s -> %s (не число)\n", arg, arg);
    }
}