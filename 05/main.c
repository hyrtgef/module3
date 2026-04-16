#include "counter.h"

int init_program(void);
void run_main_loop(void);
void cleanup_program(void);

int main() {
    if (!init_program()) {
        return 1;
    }
    
    run_main_loop();
    
    cleanup_program();
    
    return 0;
}