#ifndef CONTACT_H
#define CONTACT_H

#define MAX_NAME_LEN     64
#define MAX_WORK_LEN     64
#define MAX_POSITION_LEN 64
#define MAX_PHONE_LEN    20
#define MAX_EMAIL_LEN    64
#define MAX_SOCIAL_LEN   64
#define MAX_MESSENGER_LEN 64
#define MAX_CONTACTS     1000

typedef struct {
    char lastName[MAX_NAME_LEN];
    char firstName[MAX_NAME_LEN];
    char middleName[MAX_NAME_LEN];   
    char work[MAX_WORK_LEN];
    char position[MAX_POSITION_LEN];
    char phone[MAX_PHONE_LEN];
    char email[MAX_EMAIL_LEN];
    char social[MAX_SOCIAL_LEN];
    char messenger[MAX_MESSENGER_LEN];
} Contact;

int loadContacts(const char *filename, Contact *contacts, int maxContacts);

int saveContacts(const char *filename, const Contact *contacts, int count);

int addContact(Contact *contacts, int *count, const Contact *newContact);

int editContact(Contact *contacts, int count, int index, const Contact *newData);

int deleteContact(Contact *contacts, int *count, int index);

int findContact(const Contact *contacts, int count, const char *lastName, const char *firstName);

void printAllContacts(const Contact *contacts, int count);

void printContact(const Contact *c, int index);

#endif