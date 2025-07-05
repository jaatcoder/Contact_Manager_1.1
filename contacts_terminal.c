#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Define the absolute path to the contacts file
#define CONTACTS_FILE "/Users/mubinsompod/Documents/Code/c++/project_1-1/contacts.txt"
#define MAX_CONTACTS 500

typedef struct {
    char name[50];
    char contact[15];
    char email[50];
    char group[20];
} Contact;

// Function prototypes
void terminal_add_contact();
void terminal_show_contacts();
void terminal_search_contact();
void terminal_delete_contact();
void terminal_show_by_group();
int delete_contact_from_file(const char *name, const char *contact);
void get_unique_groups(char groups[][20], int *count);
int compare_contacts(const void *a, const void *b);

// Compare contacts for sorting
int compare_contacts(const void *a, const void *b) {
    Contact *contactA = (Contact *)a;
    Contact *contactB = (Contact *)b;
    return strcasecmp(contactA->name, contactB->name);
}

// Function to delete a contact from the file
int delete_contact_from_file(const char *name, const char *contact) {
    if (!name || !contact) return 0;
    
    // Read all contacts
    Contact contacts[MAX_CONTACTS];
    int count = 0;
    
    FILE *file = fopen(CONTACTS_FILE, "r");
    if (file == NULL) {
        return 0;
    }
    
    char line[256];
    while (!feof(file) && count < MAX_CONTACTS) {
        // Read name
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = 0;
        strncpy(contacts[count].name, line, sizeof(contacts[count].name) - 1);
        contacts[count].name[sizeof(contacts[count].name) - 1] = '\0';
        
        // Read contact
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = 0;
        strncpy(contacts[count].contact, line, sizeof(contacts[count].contact) - 1);
        contacts[count].contact[sizeof(contacts[count].contact) - 1] = '\0';
        
        // Read email
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = 0;
        strncpy(contacts[count].email, line, sizeof(contacts[count].email) - 1);
        contacts[count].email[sizeof(contacts[count].email) - 1] = '\0';
        
        // Read group
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = 0;
        strncpy(contacts[count].group, line, sizeof(contacts[count].group) - 1);
        contacts[count].group[sizeof(contacts[count].group) - 1] = '\0';
        
        count++;
    }
    
    fclose(file);
    
    // Write back all contacts except the one to delete
    file = fopen(CONTACTS_FILE, "w");
    if (file == NULL) {
        return 0;
    }
    
    int deleted = 0;
    for (int i = 0; i < count; i++) {
        // Skip the contact we want to delete
        if (strcmp(contacts[i].name, name) == 0 && strcmp(contacts[i].contact, contact) == 0) {
            deleted = 1;
            continue;
        }
        
        fprintf(file, "%s\n%s\n%s\n%s\n", 
                contacts[i].name, contacts[i].contact, 
                contacts[i].email, contacts[i].group);
    }
    
    fclose(file);
    return deleted;
}

// Function to collect unique groups from contacts file
void get_unique_groups(char groups[][20], int *count) {
    FILE *file = fopen(CONTACTS_FILE, "r");
    if (file == NULL) return;
    
    *count = 0;
    char line[256];
    char name[50], contact[15], email[50], group[20];
    
    while (!feof(file)) {
        // Skip name
        if (fgets(line, sizeof(line), file) == NULL) break;
        
        // Skip contact
        if (fgets(line, sizeof(line), file) == NULL) break;
        
        // Skip email
        if (fgets(line, sizeof(line), file) == NULL) break;
        
        // Read group
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = 0;
        
        // Check if we already have this group
        int exists = 0;
        for (int i = 0; i < *count; i++) {
            if (strcmp(groups[i], line) == 0) {
                exists = 1;
                break;
            }
        }
        
        // Add new group if it doesn't exist
        if (!exists && *count < MAX_CONTACTS) {
            strncpy(groups[*count], line, 19);
            groups[*count][19] = '\0';  // Ensure null termination
            (*count)++;
        }
    }
    
    fclose(file);
}

void terminal_add_contact() {
    FILE *file = fopen(CONTACTS_FILE, "a");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open contacts file.\n");
        return;
    }

    char name[50], email[50], contact[15], group[20];
    
    printf("Enter Name: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';
    
    printf("Enter Contact Number: ");
    fgets(contact, sizeof(contact), stdin);
    contact[strcspn(contact, "\n")] = '\0';
    
    printf("Enter Email: ");
    fgets(email, sizeof(email), stdin);
    email[strcspn(email, "\n")] = '\0';
    
    printf("Enter Group: ");
    fgets(group, sizeof(group), stdin);
    group[strcspn(group, "\n")] = '\0';

    fprintf(file, "%s\n%s\n%s\n%s\n", name, contact, email, group);
    fclose(file);

    printf("Contact added successfully!\n");
}

void terminal_show_contacts() {
    FILE *file = fopen(CONTACTS_FILE, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open contacts file.\n");
        return;
    }

    Contact contacts[MAX_CONTACTS]; // Support up to MAX_CONTACTS contacts
    int count = 0;

    char line[256];
    while (!feof(file) && count < MAX_CONTACTS) {
        // Read name
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = '\0';
        strncpy(contacts[count].name, line, sizeof(contacts[count].name) - 1);
        contacts[count].name[sizeof(contacts[count].name) - 1] = '\0';
        
        // Read contact
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = '\0';
        strncpy(contacts[count].contact, line, sizeof(contacts[count].contact) - 1);
        contacts[count].contact[sizeof(contacts[count].contact) - 1] = '\0';
        
        // Read email
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = '\0';
        strncpy(contacts[count].email, line, sizeof(contacts[count].email) - 1);
        contacts[count].email[sizeof(contacts[count].email) - 1] = '\0';
        
        // Read group
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = '\0';
        strncpy(contacts[count].group, line, sizeof(contacts[count].group) - 1);
        contacts[count].group[sizeof(contacts[count].group) - 1] = '\0';
        
        count++;
    }

    fclose(file);

    // Sort contacts by name
    qsort(contacts, count, sizeof(Contact), compare_contacts);

    printf("----- Contact List (%d contacts) -----\n", count);
    for (int i = 0; i < count; i++) {
        printf("%d. Name: %s\n   Contact: %s\n   Email: %s\n   Group: %s\n\n", 
               i + 1, contacts[i].name, contacts[i].contact, contacts[i].email, contacts[i].group);
    }
}

void terminal_search_contact() {
    FILE *file = fopen(CONTACTS_FILE, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open contacts file.\n");
        return;
    }

    char search[256];
    printf("Enter search term: ");
    fgets(search, sizeof(search), stdin);
    search[strcspn(search, "\n")] = '\0';

    Contact results[MAX_CONTACTS]; // Support up to MAX_CONTACTS results
    int found = 0;

    char line[256];
    char name[50], contact[15], email[50], group[20];
    
    while (!feof(file) && found < MAX_CONTACTS) {
        // Read name
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = '\0';
        strncpy(name, line, sizeof(name) - 1);
        name[sizeof(name) - 1] = '\0';
        
        // Read contact
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = '\0';
        strncpy(contact, line, sizeof(contact) - 1);
        contact[sizeof(contact) - 1] = '\0';
        
        // Read email
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = '\0';
        strncpy(email, line, sizeof(email) - 1);
        email[sizeof(email) - 1] = '\0';
        
        // Read group
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = '\0';
        strncpy(group, line, sizeof(group) - 1);
        group[sizeof(group) - 1] = '\0';
        
        if (
            strcasestr(name, search) ||
            strcasestr(contact, search) ||
            strcasestr(email, search) ||
            strcasestr(group, search)
        ) {
            strcpy(results[found].name, name);
            strcpy(results[found].contact, contact);
            strcpy(results[found].email, email);
            strcpy(results[found].group, group);
            found++;
        }
    }
    
    fclose(file);

    if (found == 0) {
        printf("No matching contacts found.\n");
        return;
    }

    // Sort results by name
    qsort(results, found, sizeof(Contact), compare_contacts);

    printf("\n--- Search Results (%d contacts) ---\n", found);
    for (int i = 0; i < found; i++) {
        printf("%d. Name: %s\n   Contact: %s\n   Email: %s\n   Group: %s\n\n",
               i + 1, results[i].name, results[i].contact, results[i].email, results[i].group);
    }
}

// Function to show contacts by group
void terminal_show_by_group() {
    // First get all unique groups
    char groups[MAX_CONTACTS][20];
    int group_count = 0;
    get_unique_groups(groups, &group_count);
    
    // Display available groups
    printf("Available groups:\n");
    for (int i = 0; i < group_count; i++) {
        printf("%d. %s\n", i + 1, groups[i]);
    }
    
    // Ask user to select a group
    int choice;
    printf("\nEnter group number (1-%d): ", group_count);
    if (scanf("%d", &choice) != 1 || choice < 1 || choice > group_count) {
        printf("Invalid choice!\n");
        // Clear input buffer
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        return;
    }
    
    // Clear input buffer
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    
    // Selected group
    char selected_group[20];
    strcpy(selected_group, groups[choice - 1]);
    
    // Now load contacts and filter by the selected group
    FILE *file = fopen(CONTACTS_FILE, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open contacts file.\n");
        return;
    }

    Contact filtered_contacts[MAX_CONTACTS];
    int filtered_count = 0;

    char line[256];
    char name[50], contact[15], email[50], group[20];
    
    while (!feof(file) && filtered_count < MAX_CONTACTS) {
        // Read name
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = '\0';
        strncpy(name, line, sizeof(name) - 1);
        name[sizeof(name) - 1] = '\0';
        
        // Read contact
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = '\0';
        strncpy(contact, line, sizeof(contact) - 1);
        contact[sizeof(contact) - 1] = '\0';
        
        // Read email
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = '\0';
        strncpy(email, line, sizeof(email) - 1);
        email[sizeof(email) - 1] = '\0';
        
        // Read group
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = '\0';
        strncpy(group, line, sizeof(group) - 1);
        group[sizeof(group) - 1] = '\0';
        
        // Check if this contact belongs to the selected group
        if (strcasecmp(group, selected_group) == 0) {
            strcpy(filtered_contacts[filtered_count].name, name);
            strcpy(filtered_contacts[filtered_count].contact, contact);
            strcpy(filtered_contacts[filtered_count].email, email);
            strcpy(filtered_contacts[filtered_count].group, group);
            filtered_count++;
        }
    }
    
    fclose(file);
    
    // Sort the filtered contacts
    qsort(filtered_contacts, filtered_count, sizeof(Contact), compare_contacts);
    
    // Display the filtered contacts
    printf("\n----- Contacts in group \"%s\" (%d contacts) -----\n", selected_group, filtered_count);
    for (int i = 0; i < filtered_count; i++) {
        printf("%d. Name: %s\n   Contact: %s\n   Email: %s\n   Group: %s\n\n", 
               i + 1, filtered_contacts[i].name, filtered_contacts[i].contact, 
               filtered_contacts[i].email, filtered_contacts[i].group);
    }
}

// Function to delete a contact from terminal
void terminal_delete_contact() {
    // First show all contacts with numbers
    FILE *file = fopen(CONTACTS_FILE, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open contacts file.\n");
        return;
    }

    Contact contacts[MAX_CONTACTS]; // Support up to MAX_CONTACTS contacts
    int count = 0;

    char line[256];
    while (!feof(file) && count < MAX_CONTACTS) {
        // Read name
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = '\0';
        strncpy(contacts[count].name, line, sizeof(contacts[count].name) - 1);
        contacts[count].name[sizeof(contacts[count].name) - 1] = '\0';
        
        // Read contact
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = '\0';
        strncpy(contacts[count].contact, line, sizeof(contacts[count].contact) - 1);
        contacts[count].contact[sizeof(contacts[count].contact) - 1] = '\0';
        
        // Read email
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = '\0';
        strncpy(contacts[count].email, line, sizeof(contacts[count].email) - 1);
        contacts[count].email[sizeof(contacts[count].email) - 1] = '\0';
        
        // Read group
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = '\0';
        strncpy(contacts[count].group, line, sizeof(contacts[count].group) - 1);
        contacts[count].group[sizeof(contacts[count].group) - 1] = '\0';
        
        count++;
    }

    fclose(file);

    // Sort contacts by name
    qsort(contacts, count, sizeof(Contact), compare_contacts);

    printf("----- Contact List (%d contacts) -----\n", count);
    for (int i = 0; i < count; i++) {
        printf("%d. Name: %s\n   Contact: %s\n   Email: %s\n   Group: %s\n\n", 
               i + 1, contacts[i].name, contacts[i].contact, contacts[i].email, contacts[i].group);
    }
    
    // Ask user which contact to delete
    int choice;
    printf("Enter number of the contact to delete (1-%d) or 0 to cancel: ", count);
    if (scanf("%d", &choice) != 1 || choice < 0 || choice > count) {
        printf("Invalid choice!\n");
        // Clear input buffer
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        return;
    }
    
    // Clear input buffer
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    
    if (choice == 0) {
        printf("Deletion cancelled.\n");
        return;
    }
    
    // Confirm deletion
    char confirm;
    printf("Are you sure you want to delete %s? (y/n): ", contacts[choice-1].name);
    scanf("%c", &confirm);
    
    // Clear input buffer
    while ((c = getchar()) != '\n' && c != EOF);
    
    if (confirm == 'y' || confirm == 'Y') {
        if (delete_contact_from_file(contacts[choice-1].name, contacts[choice-1].contact)) {
            printf("Contact deleted successfully!\n");
        } else {
            printf("Failed to delete contact.\n");
        }
    } else {
        printf("Deletion cancelled.\n");
    }
}

int main() {
    int choice;

    while (1) {
        printf("\n====== Contact Manager ======\n");
        printf("1. Add Contact\n");
        printf("2. Show All Contacts\n");
        printf("3. Search Contact\n");
        printf("4. Show Contacts by Group\n");
        printf("5. Delete Contact\n");
        printf("6. Exit\n");
        printf("Enter choice: ");
        
        if (scanf("%d", &choice) != 1) {
            // Clear the input buffer if invalid input
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            printf("Invalid input. Please enter a number.\n");
            continue;
        }
        
        // Consume newline
        getchar();

        switch (choice) {
            case 1:
                terminal_add_contact();
                break;
            case 2:
                terminal_show_contacts();
                break;
            case 3:
                terminal_search_contact();
                break;
            case 4:
                terminal_show_by_group();
                break;
            case 5:
                terminal_delete_contact();
                break;
            case 6:
                printf("Exiting program. Have a good day!\n");
                return 0;
            default:
                printf("Invalid choice! Try again.\n");
        }
    }
    
    return 0;
} 