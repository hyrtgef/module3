#include "chat.h"

static mqd_t g_mq_send = (mqd_t)-1;
static mqd_t g_mq_receive = (mqd_t)-1;
static const char *g_send_queue = QUEUE_NAME_2_1;
static const char *g_receive_queue = QUEUE_NAME_1_2;
static int g_running = 1;

void signal_handler(int signum) {
    (void)signum;  
    printf("\n[Получен сигнал завершения]\n");
    if (g_mq_send != (mqd_t)-1) {
        send_chat_message(g_mq_send, "Завершение чата", 1);
    }
    g_running = 0;
}

void print_help() {
    printf("\n=== Чат Пользователь 2 ===\n");  
    printf("Команды:\n");
    printf("  /quit  - завершить чат\n");
    printf("  /help  - показать справку\n");
    printf("  Любое другое сообщение будет отправлено Пользователю 1\n");
    printf("============================\n\n");
}

int main() {
    char input[MAX_MSG_SIZE];
    char received[MAX_MSG_SIZE];
    int result;
    struct mq_attr attr;
    
    setup_signal_handler(signal_handler);
    
    printf("Инициализация чата для Пользователя 2...\n");  
    
    if (init_chat_queues(g_send_queue, g_receive_queue, &g_mq_send, &g_mq_receive) == -1) {
        fprintf(stderr, "Ошибка инициализации очередей\n");
        return 1;
    }
    
    print_help();
    printf("[Ожидание сообщения от Пользователя 1...]\n");  
    
    while (g_running) {
        if (mq_getattr(g_mq_receive, &attr) == 0 && attr.mq_curmsgs > 0) {
            result = receive_chat_message(g_mq_receive, received, MAX_MSG_SIZE);
            
            if (result == 1) {
                printf("\n[Пользователь 1 завершил чат]\n");
                break;
            } else if (result == 0) {
                printf("\n[Пользователь 1]: %s\n", received); 
                printf("[Вы]: ");
                fflush(stdout);
            }
        }
        
        fd_set fds;
        struct timeval tv;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        tv.tv_sec = 0;
        tv.tv_usec = 100000;
        
        if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0) {
            if (FD_ISSET(STDIN_FILENO, &fds)) {
                if (fgets(input, MAX_MSG_SIZE, stdin) != NULL) {
                    input[strcspn(input, "\n")] = 0;
                    
                    if (strlen(input) == 0) {
                        continue;
                    }
                    
                    if (strcmp(input, "/quit") == 0) {
                        printf("[Завершение чата...]\n");
                        send_chat_message(g_mq_send, "Завершение чата", 1);
                        break;
                    } else if (strcmp(input, "/help") == 0) {
                        print_help();
                        printf("[Вы]: ");
                        fflush(stdout);
                    } else {
                        send_chat_message(g_mq_send, input, 0);
                        printf("[Сообщение отправлено]\n[Вы]: ");
                        fflush(stdout);
                    }
                }
            }
        }
    }
    
    cleanup_chat(g_mq_send, g_mq_receive, g_send_queue, g_receive_queue);
    return 0;
}