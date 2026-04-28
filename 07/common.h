#ifndef COMMON_H
#define COMMON_H


#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include<signal.h>
#include <unistd.h>
#include <errno.h>
#include<sys/select.h>

#define QUEUE_NAME_1_2 "/chat_queue_1_to_2"
#define QUEUE_NAME_2_1 "/chat_queue_2_to_1"

#define PRIORITY_NORMAL 1     
#define PRIORITY_TERMINATE 99 
#define MAX_MSG_SIZE 1024

#define MAX_MESSAGES 10

typedef struct {
    char text[MAX_MSG_SIZE];
} chat_message_t;

mqd_t create_or_open_queue(const char *name, int flags);
void close_and_unlink_queue(mqd_t mq, const char *name);
void send_message(mqd_t mq, const char *msg, unsigned int priority);
ssize_t receive_message(mqd_t mq, char *buffer, size_t size, unsigned int *priority);

#endif 