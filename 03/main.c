#include "phonebook.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DATA_FILE "data.bin"

void safeInput(char *buffer, int maxLen, const char *prompt) {
    printf("%s", prompt);
    fgets(buffer, maxLen, stdin);
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    }
}

int inputField(char *dest, const char *fieldName, const char *oldValue) {
    char buffer[MAX_NAME_LEN];
    printf("%s (%s): ", fieldName, oldValue);
    fgets(buffer, sizeof(buffer), stdin);
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    }
    
    if (strlen(buffer) > 0) {
        strcpy(dest, buffer);
        return 1;
    }
    return 0;
}

int inputRequiredFields(char *lastName, char *firstName) {
    char buffer[MAX_NAME_LEN];
    
    while (1) {
        printf("Фамилия: ");
        fgets(buffer, sizeof(buffer), stdin);
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
        
        if (strlen(buffer) == 0) {
            printf("Ошибка: Фамилия обязательна для заполнения! Попробуйте снова.\n");
            continue;
        }
        strcpy(lastName, buffer);
        break;
    }
    
    while (1) {
        printf("Имя: ");
        fgets(buffer, sizeof(buffer), stdin);
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
        
        if (strlen(buffer) == 0) {
            printf("Ошибка: Имя обязательно для заполнения! Попробуйте снова.\n");
            continue;
        }
        strcpy(firstName, buffer);
        break;
    }
    
    return 1;
}

int createContact(Contact *c) {
    memset(c, 0, sizeof(Contact));
    
    if (!inputRequiredFields(c->lastName, c->firstName)) {
        return 0;
    }
    
    safeInput(c->middleName, MAX_NAME_LEN, "Отчество (Enter - пропустить): ");
    safeInput(c->work, MAX_WORK_LEN, "Место работы (Enter - пропустить): ");
    safeInput(c->position, MAX_POSITION_LEN, "Должность (Enter - пропустить): ");
    safeInput(c->phone, MAX_PHONE_LEN, "Телефон (Enter - пропустить): ");
    safeInput(c->email, MAX_EMAIL_LEN, "Email (Enter - пропустить): ");
    safeInput(c->social, MAX_SOCIAL_LEN, "Ссылка на соцсеть (Enter - пропустить): ");
    safeInput(c->messenger, MAX_MESSENGER_LEN, "Мессенджер (Enter - пропустить): ");
    
    return 1;
}

void editContactSelective(Contact *old, Contact *newContact) {
    *newContact = *old;
    
    printf("\n=== Редактирование контакта ===\n");
    printf("Для пропуска поля просто нажмите Enter\n\n");
    
    char buffer[MAX_NAME_LEN];
    
    printf("Фамилия (текущая: %s)\n", old->lastName);
    while (1) {
        printf("Новая фамилия (Enter - оставить без изменений): ");
        fgets(buffer, sizeof(buffer), stdin);
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
        
        if (strlen(buffer) == 0) {
            break; 
        }
        
        strcpy(newContact->lastName, buffer);
        break;
    }
    
    printf("Имя (текущее: %s)\n", old->firstName);
    while (1) {
        printf("Новое имя (Enter - оставить без изменений): ");
        fgets(buffer, sizeof(buffer), stdin);
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
        
        if (strlen(buffer) == 0) {
            break;
        }
        
        strcpy(newContact->firstName, buffer);
        break;
    }
    
    inputField(newContact->middleName, "Отчество", old->middleName);
    inputField(newContact->work, "Место работы", old->work);
    inputField(newContact->position, "Должность", old->position);
    inputField(newContact->phone, "Телефон", old->phone);
    inputField(newContact->email, "Email", old->email);
    inputField(newContact->social, "Соцсеть", old->social);
    inputField(newContact->messenger, "Мессенджер", old->messenger);
}

int main() {
    Contact contacts[MAX_CONTACTS];
    int contactCount = 0;
    
    int loaded = loadContacts(DATA_FILE, contacts, MAX_CONTACTS);
    if (loaded < 0) {
        printf("Ошибка чтения файла! Начинаем с пустой книги.\n");
        contactCount = 0;
    } else {
        contactCount = loaded;
        printf("Загружено %d контактов из файла.\n", contactCount);
    }
    
    int choice;
    do {
        printf("\n=== Телефонная книга ===\n");
        printf("1. Показать все контакты\n");
        printf("2. Добавить контакт\n");
        printf("3. Редактировать контакт\n");
        printf("4. Удалить контакт\n");
        printf("5. Найти контакт\n");
        printf("0. Выход и сохранение\n");
        printf("Ваш выбор: ");
        
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            choice = -1;
        }
        while (getchar() != '\n');
        
        switch (choice) {
            case 1:
                printAllContacts(contacts, contactCount);
                break;
                
            case 2: {
                if (contactCount >= MAX_CONTACTS) {
                    printf("Достигнуто максимальное количество контактов (%d)!\n", MAX_CONTACTS);
                    break;
                }
                Contact newContact;
                if (createContact(&newContact)) {
                    addContact(contacts, &contactCount, &newContact);
                    printf("Контакт добавлен!\n");
                }
                break;
            }
            
            case 3: {
                if (contactCount == 0) {
                    printf("Нет контактов для редактирования.\n");
                    break;
                }
                int idx;
                printf("Введите номер контакта для редактирования (1-%d): ", contactCount);
                if (scanf("%d", &idx) == 1 && idx >= 1 && idx <= contactCount) {
                    while (getchar() != '\n');
                    
                    printf("\nТекущий контакт:\n");
                    printContact(&contacts[idx - 1], idx - 1);
                    
                    Contact editedContact;
                    editContactSelective(&contacts[idx - 1], &editedContact);
                    
                    editContact(contacts, contactCount, idx - 1, &editedContact);
                    printf("\nКонтакт обновлён!\n");
                    
                    printf("\nОбновлённый контакт:\n");
                    printContact(&contacts[idx - 1], idx - 1);
                } else {
                    printf("Неверный номер!\n");
                    while (getchar() != '\n');
                }
                break;
            }
            
            case 4: {
                if (contactCount == 0) {
                    printf("Нет контактов для удаления.\n");
                    break;
                }
                int idx;
                printf("Введите номер контакта для удаления (1-%d): ", contactCount);
                if (scanf("%d", &idx) == 1 && idx >= 1 && idx <= contactCount) {
                    while (getchar() != '\n');
                    printf("Контакт будет удалён:\n");
                    printContact(&contacts[idx - 1], idx - 1);
                    printf("Подтвердите удаление (y/n): ");
                    char confirm;
                    scanf("%c", &confirm);
                    while (getchar() != '\n');
                    if (confirm == 'y' || confirm == 'Y') {
                        deleteContact(contacts, &contactCount, idx - 1);
                        printf("Контакт удалён!\n");
                    } else {
                        printf("Удаление отменено.\n");
                    }
                } else {
                    printf("Неверный номер!\n");
                    while (getchar() != '\n');
                }
                break;
            }
            
            case 5: {
                if (contactCount == 0) {
                    printf("Нет контактов для поиска.\n");
                    break;
                }
                char lastName[MAX_NAME_LEN], firstName[MAX_NAME_LEN];
                printf("Поиск контакта:\n");
                inputRequiredFields(lastName, firstName);
                int found = findContact(contacts, contactCount, lastName, firstName);
                if (found >= 0) {
                    printContact(&contacts[found], found);
                } else {
                    printf("Контакт не найден.\n");
                }
                break;
            }
            
            case 0:
                printf("Сохранение контактов...\n");
                if (saveContacts(DATA_FILE, contacts, contactCount) == 0) {
                    printf("Сохранено %d контактов. Выход.\n", contactCount);
                } else {
                    printf("Ошибка сохранения!\n");
                }
                break;
                
            default:
                printf("Неверный выбор. Попробуйте снова.\n");
        }
    } while (choice != 0);
    
    return 0;
}