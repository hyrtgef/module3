#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <errno.h>

#define SERVER_PRIORITY 10
#define CLIENT_PRIORITY_START 20
#define CLIENT_PRIORITY_STEP 10
#define MAX_CLIENTS 100
#define MAX_MSG_SIZE 1024
#define PROJECT_ID 1234

struct chat_message {
    long mtype;                 
    int sender_priority;       
    char text[MAX_MSG_SIZE];    
};

key_t get_queue_key(void);

int is_valid_client_priority(int priority);

#endif 