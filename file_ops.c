#include "contact_manager.h"

int saveContacts() {
    FILE *file = fopen(FILENAME, "w");
    if (file == NULL) {
        printf("\nError: Unable to save contacts! Please check file permissions.\n");
        return 0;
    }

    // Write CSV header if file is empty
    if (ftell(file) == 0) {
        fprintf(file, "Name,Phone,Email\n");
    }

    int success = 1;
    for (int i = 0; i < contactCount; i++) {
        // Escape commas in fields
        char escapedName[MAX_NAME * 2];
        char escapedPhone[MAX_PHONE * 2];
        char escapedEmail[MAX_EMAIL * 2];

        snprintf(escapedName, sizeof(escapedName), "\"%s\"", contacts[i].name);
        snprintf(escapedPhone, sizeof(escapedPhone), "\"%s\"", contacts[i].phone);
        snprintf(escapedEmail, sizeof(escapedEmail), "\"%s\"", contacts[i].email);

        if (fprintf(file, "%s,%s,%s\n", escapedName, escapedPhone, escapedEmail) < 0) {
            printf("\nError: Failed to write contact %d\n", i + 1);
            success = 0;
            break;
        }
    }

    fclose(file);
    if (success) {
        printf("\nContacts saved successfully!\n");
    }
    return success;
}

int loadContacts() {
    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) {
        printf("\nNo existing contacts file found. Starting fresh.\n");
        return 1; // Not an error condition for first run
    }

    char line[MAX_NAME + MAX_PHONE + MAX_EMAIL + 3]; // +3 for commas and newline
    contactCount = 0;

    // Skip header line
    fgets(line, sizeof(line), file);

    while (fgets(line, sizeof(line), file) && contactCount < MAX_CONTACTS) {
        Contact *current = &contacts[contactCount];
        char *pos = line;
        int field = 0;

        while (field < 3) {
            char value[MAX_NAME]; // Use MAX_NAME as it's the largest field size
            int i = 0;

            // Skip leading whitespace
            while (*pos && isspace(*pos)) pos++;

            // Handle quoted values
            if (*pos == '"') {
                pos++; // Skip opening quote
                while (*pos && *pos != '"' && i < MAX_NAME - 1) {
                    value[i++] = *pos++;
                }
                if (*pos == '"') pos++; // Skip closing quote
            } else {
                while (*pos && *pos != ',' && *pos != '\n' && i < MAX_NAME - 1) {
                    value[i++] = *pos++;
                }
            }
            value[i] = '\0';

            // Store value in appropriate field
            switch (field) {
                case 0:
                    strncpy(current->name, value, MAX_NAME - 1);
                    current->name[MAX_NAME - 1] = '\0';
                    break;
                case 1:
                    strncpy(current->phone, value, MAX_PHONE - 1);
                    current->phone[MAX_PHONE - 1] = '\0';
                    break;
                case 2:
                    strncpy(current->email, value, MAX_EMAIL - 1);
                    current->email[MAX_EMAIL - 1] = '\0';
                    break;
            }

            field++;
            if (*pos == ',') pos++;
        }

        contactCount++;
    }

    fclose(file);
    printf("\nLoaded %d contacts successfully!\n", contactCount);
    return 1;
}