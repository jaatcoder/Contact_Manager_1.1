#include "contact_manager.h"

int main() {
    loadContacts();
    int choice;

    while (1) {
        clearScreen();
        displayMenu();
        
        if (scanf("%d", &choice) != 1) {
            printf("\nInvalid input! Please enter a number.\n");
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }
        getchar(); // Clear newline after number

        switch (choice) {
            case 1:
                addContact();
                break;
            case 2:
                viewContacts();
                break;
            case 3:
                searchContact();
                break;
            case 4:
                deleteContact();
                break;
            case 5:
                printf("\nThank you for using Contact Manager!\n");
                return 0;
            default:
                printf("\nInvalid choice! Please try again.\n");
        }

        printf("\nPress Enter to continue...");
        getchar();
    }

    return 0;
}
