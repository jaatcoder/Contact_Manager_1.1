#include "contact_manager.h"

Contact contacts[MAX_CONTACTS];
int contactCount = 0;

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void displayMenu() {
    printf("\n=== Contact Management System ===\n");
    printf("1. Add Contact\n");
    printf("2. View All Contacts\n");
    printf("3. Search Contact\n");
    printf("4. Delete Contact\n");
    printf("5. Exit\n");
    printf("===============================\n");
    printf("Enter your choice (1-5): ");
}

void addContact() {
    if (contactCount >= MAX_CONTACTS) {
        printf("\nError: Contact list is full! Maximum contacts: %d\n", MAX_CONTACTS);
        return;
    }

    Contact newContact;
    printf("\n=== Add New Contact ===\n");

    do {
        printf("Enter name (1-%d characters): ", MAX_NAME - 1);
        if (fgets(newContact.name, MAX_NAME, stdin) == NULL) {
            printf("Error reading input. Please try again.\n");
            continue;
        }
        cleanupString(newContact.name);
    } while (strlen(newContact.name) == 0);

    do {
        printf("Enter phone number (format: +XX-XXX-XXX-XXXX): ");
        if (fgets(newContact.phone, MAX_PHONE, stdin) == NULL) {
            printf("Error reading input. Please try again.\n");
            continue;
        }
        cleanupString(newContact.phone);
    } while (!validatePhone(newContact.phone));

    do {
        printf("Enter email (example: name@domain.com): ");
        if (fgets(newContact.email, MAX_EMAIL, stdin) == NULL) {
            printf("Error reading input. Please try again.\n");
            continue;
        }
        cleanupString(newContact.email);
    } while (!validateEmail(newContact.email));

    contacts[contactCount++] = newContact;
    printf("\nContact added successfully!\n");
    if (!saveContacts()) {
        printf("Warning: Failed to save contacts to file.\n");
    }
}

void viewContacts() {
    if (contactCount == 0) {
        printf("\nNo contacts found!\n");
        return;
    }

    printf("\n=== Contact List (%d contacts) ===\n", contactCount);
    printf("%-4s %-20s %-15s %-30s\n", "No.", "Name", "Phone", "Email");
    printf("------------------------------------------------------------\n");

    for (int i = 0; i < contactCount; i++) {
        printf("%-4d %-20s %-15s %-30s\n", 
               i + 1, 
               contacts[i].name, 
               contacts[i].phone, 
               contacts[i].email);
    }
}

void searchContact() {
    if (contactCount == 0) {
        printf("\nNo contacts found!\n");
        return;
    }

    char searchName[MAX_NAME];
    printf("\nEnter name to search: ");
    if (fgets(searchName, MAX_NAME, stdin) == NULL) {
        printf("Error reading input.\n");
        return;
    }
    cleanupString(searchName);

    if (strlen(searchName) == 0) {
        printf("Search term cannot be empty.\n");
        return;
    }

    printf("\n=== Search Results ===\n");
    printf("%-4s %-20s %-15s %-30s\n", "No.", "Name", "Phone", "Email");
    printf("------------------------------------------------------------\n");

    int found = 0;
    for (int i = 0; i < contactCount; i++) {
        if (strstr(contacts[i].name, searchName) != NULL) {
            printf("%-4d %-20s %-15s %-30s\n", 
                   i + 1, 
                   contacts[i].name, 
                   contacts[i].phone, 
                   contacts[i].email);
            found++;
        }
    }

    printf("\nFound %d matching contact(s)\n", found);
}

void deleteContact() {
    if (contactCount == 0) {
        printf("\nNo contacts to delete!\n");
        return;
    }

    viewContacts();

    char input[10];
    int index;

    printf("\nEnter the number of contact to delete (1-%d): ", contactCount);
    if (fgets(input, sizeof(input), stdin) == NULL) {
        printf("Error reading input.\n");
        return;
    }

    if (sscanf(input, "%d", &index) != 1) {
        printf("\nInvalid input! Please enter a number.\n");
        return;
    }

    if (index < 1 || index > contactCount) {
        printf("\nInvalid contact number! Please enter a number between 1 and %d.\n", contactCount);
        return;
    }

    index--; // Convert to 0-based index
    printf("\nDeleting contact: %s\n", contacts[index].name);
    printf("Are you sure? (y/n): ");

    if (fgets(input, sizeof(input), stdin) == NULL || input[0] != 'y') {
        printf("Deletion cancelled.\n");
        return;
    }

    for (int i = index; i < contactCount - 1; i++) {
        contacts[i] = contacts[i + 1];
    }
    contactCount--;

    printf("\nContact deleted successfully!\n");
    if (!saveContacts()) {
        printf("Warning: Failed to save contacts after deletion.\n");
    }
}

void cleanupString(char *str) {
    str[strcspn(str, "\n")] = 0;
    // Remove trailing whitespace
    int i = strlen(str) - 1;
    while (i >= 0 && isspace(str[i])) {
        str[i] = 0;
        i--;
    }
    // Remove leading whitespace
    char *start = str;
    while (*start && isspace(*start)) {
        start++;
    }
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
}

int validateEmail(const char *email) {
    if (strlen(email) == 0) {
        printf("Email cannot be empty!\n");
        return 0;
    }
    if (strchr(email, '@') == NULL || strchr(email, '.') == NULL) {
        printf("Invalid email format! Must contain @ and .\n");
        return 0;
    }
    return 1;
}

int validatePhone(const char *phone) {
    if (strlen(phone) == 0) {
        printf("Phone number cannot be empty!\n");
        return 0;
    }
    if (strlen(phone) < 10) {
        printf("Phone number too short! Minimum 10 digits required.\n");
        return 0;
    }
    for (int i = 0; phone[i] != '\0'; i++) {
        if (!isdigit(phone[i]) && phone[i] != '-' && phone[i] != '+') {
            printf("Invalid phone format! Use only digits, - or +\n");
            return 0;
        }
    }
    return 1;
}