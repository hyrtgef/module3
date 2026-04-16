#include "phonebook.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int loadContacts(const char *filename, Contact *contacts, int maxContacts) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        return 0;
    }
    
    int count = 0;
    ssize_t bytesRead;
    Contact temp;
    
    while (count < maxContacts) {
        bytesRead = read(fd, &temp, sizeof(Contact));
        if (bytesRead == 0) {
            break;  
        }
        if (bytesRead != sizeof(Contact)) {
            close(fd);
            return -1;  
        }
        contacts[count] = temp;
        count++;
    }
    
    close(fd);
    return count;
}

int saveContacts(const char *filename, const Contact *contacts, int count) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        return -1;
    }
    
    ssize_t bytesWritten;
    for (int i = 0; i < count; i++) {
        bytesWritten = write(fd, &contacts[i], sizeof(Contact));
        if (bytesWritten != sizeof(Contact)) {
            close(fd);
            return -1;
        }
    }
    
    close(fd);
    return 0;
}

int addContact(Contact *contacts, int *count, const Contact *newContact) {
    if (*count >= MAX_CONTACTS) {
        return -1;  // список полон
    }
    contacts[*count] = *newContact;
    (*count)++;
    return 0;
}

int editContact(Contact *contacts, int count, int index, const Contact *newData) {
    if (index < 0 || index >= count) {
        return -1;
    }
    contacts[index] = *newData;
    return 0;
}

int deleteContact(Contact *contacts, int *count, int index) {
    if (index < 0 || index >= *count) {
        return -1;
    }
    for (int i = index; i < (*count) - 1; i++) {
        contacts[i] = contacts[i + 1];
    }
    (*count)--;
    return 0;
}

int findContact(const Contact *contacts, int count, const char *lastName, const char *firstName) {
    for (int i = 0; i < count; i++) {
        if (strcmp(contacts[i].lastName, lastName) == 0 &&
            strcmp(contacts[i].firstName, firstName) == 0) {
            return i;
        }
    }
    return -1;
}

void printContact(const Contact *c, int index) {
    printf("\n--- Контакт #%d ---\n", index + 1);
    printf("Фамилия:       %s\n", c->lastName);
    printf("Имя:           %s\n", c->firstName);
    if (strlen(c->middleName) > 0)
        printf("Отчество:      %s\n", c->middleName);
    if (strlen(c->work) > 0)
        printf("Место работы:  %s\n", c->work);
    if (strlen(c->position) > 0)
        printf("Должность:     %s\n", c->position);
    if (strlen(c->phone) > 0)
        printf("Телефон:       %s\n", c->phone);
    if (strlen(c->email) > 0)
        printf("Email:         %s\n", c->email);
    if (strlen(c->social) > 0)
        printf("Соцсеть:       %s\n", c->social);
    if (strlen(c->messenger) > 0)
        printf("Мессенджер:    %s\n", c->messenger);
}

void printAllContacts(const Contact *contacts, int count) {
    if (count == 0) {
        printf("\nТелефонная книга пуста.\n");
        return;
    }
    printf("\n=== Телефонная книга (%d контактов) ===\n", count);
    for (int i = 0; i < count; i++) {
        printContact(&contacts[i], i);
    }
}