#include "server.h"

static int msgid;
static client_info clients[MAX_CLIENTS];
static int client_count = 0;

void init_server(void) {
    key_t key = get_queue_key();
    
    msgid = msgget(key, IPC_CREAT | 0666);
    if (msgid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }
    
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].active = 0;
        clients[i].priority = 0;
    }
    
    printf("=== СЕРВЕР ЗАПУЩЕН ===\n");
    printf("ID очереди: %d\n", msgid);
    printf("Приоритет сервера: %d\n", SERVER_PRIORITY);
    printf("Ожидание клиентов...\n\n");
}

void cleanup_server(void) {
    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
    } else {
        printf("\nОчередь %d удалена\n", msgid);
    }
    printf("=== СЕРВЕР ОСТАНОВЛЕН ===\n");
}

void add_client(int priority) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].priority == priority) {
            if (!clients[i].active) {
                clients[i].active = 1;
                client_count++;
                printf("[СЕРВЕР] Клиент %d реактивирован. Всего активно: %d\n", 
                       priority, client_count);
            }
            return;
        }
    }
    
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].priority == 0) {
            clients[i].priority = priority;
            clients[i].active = 1;
            client_count++;
            printf("[СЕРВЕР] Клиент %d подключился. Всего активно: %d\n", 
                   priority, client_count);
            return;
        }
    }
    
    fprintf(stderr, "[СЕРВЕР] Ошибка: достигнут лимит клиентов (%d)\n", MAX_CLIENTS);
}

void remove_client(int priority) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].priority == priority && clients[i].active) {
            clients[i].active = 0;
            client_count--;
            printf("[СЕРВЕР] Клиент %d отключился (shutdown). Всего активно: %d\n", 
                   priority, client_count);
            return;
        }
    }
}

int is_client_active(int priority) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].priority == priority) {
            return clients[i].active;
        }
    }
    return 0;
}

void broadcast_message(struct chat_message *msg, int sender_priority) {
    int sent_count = 0;
    
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && clients[i].priority != sender_priority) {
            struct chat_message broadcast_msg;
            broadcast_msg.mtype = clients[i].priority;  
            broadcast_msg.sender_priority = sender_priority;
            strncpy(broadcast_msg.text, msg->text, MAX_MSG_SIZE - 1);
            broadcast_msg.text[MAX_MSG_SIZE - 1] = '\0';
            
            if (msgsnd(msgid, &broadcast_msg, 
                      sizeof(struct chat_message) - sizeof(long), 
                      IPC_NOWAIT) == -1) {
                perror("[СЕРВЕР] Ошибка отправки клиенту");
            } else {
                sent_count++;
            }
        }
    }
    
    printf("[СЕРВЕР] Сообщение от %d переслано %d клиентам\n", 
           sender_priority, sent_count);
}

void handle_signal(int sig) {
    (void)sig;
    cleanup_server();
    exit(EXIT_SUCCESS);
}

void run_server(void) {
    struct chat_message msg;
    
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    
    while (1) {
        ssize_t recv_size = msgrcv(msgid, &msg, 
                                   sizeof(struct chat_message) - sizeof(long),
                                   SERVER_PRIORITY, 0);
        
        if (recv_size == -1) {
            if (errno == EINTR) {
                continue;
            }
            perror("[СЕРВЕР] Ошибка получения сообщения");
            break;
        }
        
        if (!is_valid_client_priority(msg.sender_priority)) {
            printf("[СЕРВЕР] Игнорируем сообщение от невалидного клиента: %d\n", 
                   msg.sender_priority);
            continue;
        }
        
        printf("[СЕРВЕР] Получено от клиента %d: %s\n", 
               msg.sender_priority, msg.text);
        
        add_client(msg.sender_priority);
        
        if (strcmp(msg.text, "shutdown") == 0) {
            remove_client(msg.sender_priority);
            
            struct chat_message confirm_msg;
            confirm_msg.mtype = msg.sender_priority;
            confirm_msg.sender_priority = SERVER_PRIORITY;
            strcpy(confirm_msg.text, "Вы отключены от чата командой shutdown");
            
            if (msgsnd(msgid, &confirm_msg, 
                      sizeof(struct chat_message) - sizeof(long), 0) == -1) {
                perror("[СЕРВЕР] Ошибка отправки подтверждения shutdown");
            }
            
            if (client_count > 0) {
                struct chat_message notify_msg;
                notify_msg.sender_priority = SERVER_PRIORITY;
                snprintf(notify_msg.text, MAX_MSG_SIZE, 
                        "*** Клиент %d покинул чат ***", msg.sender_priority);
                
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    if (clients[i].active) {
                        notify_msg.mtype = clients[i].priority;
                        msgsnd(msgid, &notify_msg, 
                              sizeof(struct chat_message) - sizeof(long), 
                              IPC_NOWAIT);
                    }
                }
            }
        } else {
            broadcast_message(&msg, msg.sender_priority);
        }
    }
}

int main(void) {
    init_server();
    run_server();
    return 0;
}