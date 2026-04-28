#ifndef SERVER_H
#define SERVER_H

#include "common.h"

typedef struct {
    int priority;
    int active;  
} client_info;

void init_server(void);
void cleanup_server(void);
void run_server(void);
void add_client(int priority);
void remove_client(int priority);
int is_client_active(int priority);
void broadcast_message(struct chat_message *msg, int sender_priority);
void handle_signal(int sig);

#endif 