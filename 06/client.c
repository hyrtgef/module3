#include "client.h"
#include <sys/select.h> 
#include <sys/wait.h>    

static int msgid;
static int client_priority;
static volatile int running = 1;
static volatile int shutdown_confirmed = 0;

void init_client(int priority) {
    if (!is_valid_client_priority(priority)) {
        fprintf(stderr, "Ошибка: Приоритет клиента должен быть %d, %d, %d...\n",
                CLIENT_PRIORITY_START, 
                CLIENT_PRIORITY_START + CLIENT_PRIORITY_STEP,
                CLIENT_PRIORITY_START + 2 * CLIENT_PRIORITY_STEP);
        exit(EXIT_FAILURE);
    }
    
    client_priority = priority;
    key_t key = get_queue_key();
    
    msgid = msgget(key, 0666);
    if (msgid == -1) {
        perror("msgget");
        fprintf(stderr, "Ошибка: Сервер не запущен. Запустите сервер сначала.\n");
        exit(EXIT_FAILURE);
    }
    
    printf("=== КЛИЕНТ %d ===\n", priority);
    printf("Подключен к очереди: %d\n", msgid);
    printf("Введите 'shutdown' для отключения\n");
}

void cleanup_client(void) {
    printf("\nКлиент %d завершил работу\n", client_priority);
}

void handle_signal(int sig) {
    (void)sig;
    running = 0;
}

void receive_messages(void) {
    struct chat_message msg;
    
    while (running && !shutdown_confirmed) {
        ssize_t recv_size = msgrcv(msgid, &msg, 
                                   sizeof(struct chat_message) - sizeof(long),
                                   client_priority, 
                                   IPC_NOWAIT);
        
        if (recv_size > 0) {
            printf("\r                                \r"); 
            
            if (msg.sender_priority == SERVER_PRIORITY) {
                printf("[СИСТЕМА]: %s\n", msg.text);
                
                if (strstr(msg.text, "отключены от чата") != NULL) {
                    shutdown_confirmed = 1;
                    running = 0;
                }
            } else {
                printf("[Клиент %d]: %s\n", msg.sender_priority, msg.text);
            }
            
            printf("Вы: ");
            fflush(stdout);
        } else if (recv_size == -1 && errno != ENOMSG) {
            if (errno != EINTR) {
                perror("Ошибка получения сообщения");
            }
            break;
        }
        
        usleep(100000); 
    }
}

void send_message(const char *text) {
    struct chat_message msg;
    msg.mtype = SERVER_PRIORITY;  
    msg.sender_priority = client_priority;
    strncpy(msg.text, text, MAX_MSG_SIZE - 1);
    msg.text[MAX_MSG_SIZE - 1] = '\0';
    
    if (msgsnd(msgid, &msg, sizeof(struct chat_message) - sizeof(long), 0) == -1) {
        perror("Ошибка отправки сообщения");
    }
}

void run_client(int priority) {
    (void)priority;
    char input[MAX_MSG_SIZE];
    fd_set fds;           
    struct timeval tv;    
    int ret;
    pid_t pid;
    
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    
    pid = fork();
    if (pid == -1) {
        perror("fork");
        return;
    }
    
    if (pid == 0) {
        receive_messages();
        exit(0);
    }
    
    printf("Вы: ");
    fflush(stdout);
    
    while (running && !shutdown_confirmed) {
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        tv.tv_sec = 0;
        tv.tv_usec = 100000;
        
        ret = select(STDIN_FILENO + 1,
&fds, NULL, NULL, &tv);
        
        if (ret > 0 && FD_ISSET(STDIN_FILENO, &fds)) {
            if (fgets(input, sizeof(input), stdin) != NULL) {
                input[strcspn(input, "\n")] = 0;
                
                if (strlen(input) > 0) {
                    send_message(input);
                    
                    if (strcmp(input, "shutdown") == 0) {
                        printf("Запрос на отключение отправлен. Ожидание подтверждения...\n");
                        sleep(2);
                        break;
                    }
                    
                    printf("Вы: ");
                    fflush(stdout);
                }
            }
        }
    }
    
    kill(pid, SIGTERM);
    waitpid(pid, NULL, 0);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <приоритет>\n", argv[0]);
        fprintf(stderr, "Пример: %s 20\n", argv[0]);
        fprintf(stderr, "Допустимые приоритеты: 20, 30, 40, ...\n");
        return EXIT_FAILURE;
    }
    
    int priority = atoi(argv[1]);
    
    init_client(priority);
    atexit(cleanup_client);
    
    run_client(priority);
    
    return 0;
}