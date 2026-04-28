#ifndef CHAT_H
#define CHAT_H

#include "common.h"

mqd_t init_chat_queues(const char *send_queue, const char *receive_queue, 
                       mqd_t *mq_send, mqd_t *mq_receive);

int send_chat_message(mqd_t mq_send, const char *message, int is_terminate);

int receive_chat_message(mqd_t mq_receive, char *buffer, size_t size);

void cleanup_chat(mqd_t mq_send, mqd_t mq_receive, 
                  const char *send_queue, const char *receive_queue);

void setup_signal_handler(void (*handler)(int));

#endif 