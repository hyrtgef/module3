#ifndef CLIENT_H
#define CLIENT_H

#include "common.h"

void init_client(int priority);
void cleanup_client(void);
void run_client(int priority);
void receive_messages(void);
void send_message(const char *text);
void handle_signal(int sig);

#endif 