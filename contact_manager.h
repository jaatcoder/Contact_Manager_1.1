#ifndef CONTACT_MANAGER_H
#define CONTACT_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NAME 50
#define MAX_PHONE 15
#define MAX_EMAIL 50
#define MAX_CONTACTS 100
#define FILENAME "contacts.csv"

typedef struct {
    char name[MAX_NAME];
    char phone[MAX_PHONE];
    char email[MAX_EMAIL];
} Contact;

// Function declarations
void clearScreen();
void displayMenu();
void addContact();
void viewContacts();
void searchContact();
void deleteContact();
int saveContacts();
int loadContacts();
void cleanupString(char *str);
int validateEmail(const char *email);
int validatePhone(const char *phone);

// Global variables
extern Contact contacts[MAX_CONTACTS];
extern int contactCount;

#endif
