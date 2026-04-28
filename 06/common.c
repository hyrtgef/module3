#include "common.h"

key_t get_queue_key(void) {
    key_t key = ftok(".", PROJECT_ID);
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }
    return key;
}

int is_valid_client_priority(int priority) {
    return (priority >= CLIENT_PRIORITY_START && 
            ((priority - CLIENT_PRIORITY_START) % CLIENT_PRIORITY_STEP == 0));
}