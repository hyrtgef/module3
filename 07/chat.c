#include "chat.h"

mqd_t create_or_open_queue(const char *name, int flags) {
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = sizeof(chat_message_t);
    attr.mq_curmsgs = 0;
    
    mqd_t mq = mq_open(name, flags, 0644, &attr);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        return (mqd_t)-1;
    }
    return mq;
}

void close_and_unlink_queue(mqd_t mq, const char *name) {
    if (mq != (mqd_t)-1) {
        mq_close(mq);
        mq_unlink(name);
    }
}

void send_message(mqd_t mq, const char *msg, unsigned int priority) {
    chat_message_t message;
    strncpy(message.text, msg, MAX_MSG_SIZE - 1);
    message.text[MAX_MSG_SIZE - 1] = '\0';
    
    if (mq_send(mq, (const char *)&message, sizeof(message), priority) == -1) {
        perror("mq_send");
    }
}

ssize_t receive_message(mqd_t mq, char *buffer, size_t size, unsigned int *priority) {
    chat_message_t message;
    ssize_t bytes_read;
    
    bytes_read = mq_receive(mq, (char *)&message, sizeof(message), priority);
    if (bytes_read == -1) {
        if (errno != EAGAIN) {
            perror("mq_receive");
        }
        return -1;
    }
    
    strncpy(buffer, message.text, size - 1);
    buffer[size - 1] = '\0';
    return bytes_read;
}

mqd_t init_chat_queues(const char *send_queue, const char *receive_queue, 
                       mqd_t *mq_send, mqd_t *mq_receive) {
    
    *mq_send = create_or_open_queue(send_queue, O_CREAT | O_WRONLY);
    if (*mq_send == (mqd_t)-1) {
        return (mqd_t)-1;
    }
    
    *mq_receive = create_or_open_queue(receive_queue, O_CREAT | O_RDONLY);
    if (*mq_receive == (mqd_t)-1) {
        close_and_unlink_queue(*mq_send, send_queue);
        return (mqd_t)-1;
    }
    
    return 0;
}

int send_chat_message(mqd_t mq_send, const char *message, int is_terminate) {
    unsigned int priority = is_terminate ? PRIORITY_TERMINATE : PRIORITY_NORMAL;
    
    send_message(mq_send, message, priority);
    
    if (is_terminate) {
        printf("[Отправлен сигнал завершения чата]\n");
    }
    
    return 0;
}

int receive_chat_message(mqd_t mq_receive, char *buffer, size_t size) {
    unsigned int priority;
    ssize_t result;
    
    result = receive_message(mq_receive, buffer, size, &priority);
    
    if (result > 0) {
        if (priority == PRIORITY_TERMINATE) {
            return 1; 
        }
        return 0; 
    }
    
    return -1; 
}

void cleanup_chat(mqd_t mq_send, mqd_t mq_receive, 
                  const char *send_queue, const char *receive_queue) {
    close_and_unlink_queue(mq_send, send_queue);
    close_and_unlink_queue(mq_receive, receive_queue);
    printf("[Чат завершен]\n");
}

void setup_signal_handler(void (*handler)(int)) {
    struct sigaction sa;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
}