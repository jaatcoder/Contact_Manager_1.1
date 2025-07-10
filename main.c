#define _GNU_SOURCE
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define CONTACTS_FILE "/Users/mubinsompod/Documents/Code/c++/project_1-1/contacts.txt"
#define MAX_CONTACTS 500

typedef struct {
    char name[50];
    char contact[15];
    char email[50];
    char group[20];
} Contact;

typedef struct _AppWidgets AppWidgets;

typedef struct {
    char *name;
    char *contact;
    AppWidgets *widgets;
} DeleteDialogData;


struct _AppWidgets {
    GtkWidget *window;
    GtkWidget *contact_list;
    GtkWidget *name_entry;
    GtkWidget *phone_entry;
    GtkWidget *email_entry;
    GtkWidget *group_entry;
    GtkWidget *search_entry;
    GtkWidget *group_filter;  
    GtkListStore *list_store;
    GtkListStore *group_store;
    GtkWidget *delete_button; 
    GtkTreeIter current_iter; 
};


void terminal_mode_main();
void terminal_add_contact();
void terminal_show_contacts();
void terminal_search_contact();
void terminal_show_by_group();
void terminal_delete_contact();


int compare_contacts(const void *a, const void *b);


void get_unique_groups(char groups[][20], int *count);



void load_groups_for_filter(AppWidgets *widgets);


int delete_contact_from_file(const char *name, const char *contact);

static void confirm_delete_response(GtkDialog *dialog, int response, gpointer user_data);
static void error_dialog_response(GtkDialog *dialog, int response, gpointer user_data);
static void warning_dialog_response(GtkDialog *dialog, int response, gpointer user_data);


static gint sort_by_name(GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer user_data) {
    gchar *name_a, *name_b;
    gint ret;
    
    gtk_tree_model_get(model, a, 0, &name_a, -1);
    gtk_tree_model_get(model, b, 0, &name_b, -1);

    if (name_a == NULL && name_b == NULL) {
        return 0;
    }
    
    if (name_a == NULL) {
        g_free(name_b);
        return 1;
    }
    
    if (name_b == NULL) {
        g_free(name_a);
        return -1;
    }
    
    ret = g_utf8_collate(g_utf8_casefold(name_a, -1), g_utf8_casefold(name_b, -1));
    
    g_free(name_a);
    g_free(name_b);
    
    return ret;
}


int compare_contacts(const void *a, const void *b) {
    Contact *contactA = (Contact *)a;
    Contact *contactB = (Contact *)b;
    return strcasecmp(contactA->name, contactB->name);
}

static void add_contact_to_list(AppWidgets *widgets, const char *name, const char *phone, const char *email, const char *group) {
    if (!name || !phone || !email || !group) return;
    
    GtkTreeIter iter;
    gtk_list_store_append(widgets->list_store, &iter);
    gtk_list_store_set(widgets->list_store, &iter,
                      0, name,
                      1, phone,
                      2, email,
                      3, group,
                      -1);

    g_print("Added contact: %s, %s, %s, %s\n", name, phone, email, group);

    gboolean group_exists = FALSE;
    GtkTreeIter group_iter;
    
    if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(widgets->group_store), &group_iter)) {
        do {
            gchar *existing_group = NULL;
            gtk_tree_model_get(GTK_TREE_MODEL(widgets->group_store), &group_iter, 0, &existing_group, -1);
            
            if (existing_group && strcmp(existing_group, group) == 0) {
                group_exists = TRUE;
                g_free(existing_group);
                break;
            }
            
            g_free(existing_group);
        } while (gtk_tree_model_iter_next(GTK_TREE_MODEL(widgets->group_store), &group_iter));
    }
    
    if (!group_exists) {
        gtk_list_store_append(widgets->group_store, &group_iter);
        gtk_list_store_set(widgets->group_store, &group_iter, 0, group, -1);
    }
}

static void save_contact_to_file(const char *name, const char *phone, const char *email, const char *group) {
    if (!name || !phone || !email || !group) return;
    
    FILE *file = fopen(CONTACTS_FILE, "a");
    if (file != NULL) {
        fprintf(file, "%s\n%s\n%s\n%s\n", name, phone, email, group);
        fclose(file);
    }
}


void load_groups_for_filter(AppWidgets *widgets) {
    GtkTreeIter iter;

    gtk_list_store_append(widgets->group_store, &iter);
    gtk_list_store_set(widgets->group_store, &iter, 0, "All Groups", -1);

    char groups[MAX_CONTACTS][20];
    int count = 0;
    get_unique_groups(groups, &count);

    for (int i = 0; i < count; i++) {
        gtk_list_store_append(widgets->group_store, &iter);
        gtk_list_store_set(widgets->group_store, &iter, 0, groups[i], -1);
    }
}


void get_unique_groups(char groups[][20], int *count) {
    FILE *file = fopen(CONTACTS_FILE, "r");
    if (file == NULL) return;
    
    *count = 0;
    char line[256];
    char name[50], contact[15], email[50], group[20];
    
    while (!feof(file)) {
        if (fgets(line, sizeof(line), file) == NULL) break;
        
        if (fgets(line, sizeof(line), file) == NULL) break;
        
        if (fgets(line, sizeof(line), file) == NULL) break;
        
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = 0;

        int exists = 0;
        for (int i = 0; i < *count; i++) {
            if (strcmp(groups[i], line) == 0) {
                exists = 1;
                break;
            }
        }
        
        if (!exists && *count < MAX_CONTACTS) {
            strncpy(groups[*count], line, 19);
            groups[*count][19] = '\0';
            (*count)++;
        }
    }
    
    fclose(file);
}


int delete_contact_from_file(const char *name, const char *contact) {
    if (!name || !contact) return 0;

    Contact contacts[MAX_CONTACTS];
    int count = 0;
    
    FILE *file = fopen(CONTACTS_FILE, "r");
    if (file == NULL) {
        return 0;
    }
    
    char line[256];
    while (!feof(file) && count < MAX_CONTACTS) {
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = 0;
        strncpy(contacts[count].name, line, sizeof(contacts[count].name) - 1);
        contacts[count].name[sizeof(contacts[count].name) - 1] = '\0';
        
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = 0;
        strncpy(contacts[count].contact, line, sizeof(contacts[count].contact) - 1);
        contacts[count].contact[sizeof(contacts[count].contact) - 1] = '\0';
        
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = 0;
        strncpy(contacts[count].email, line, sizeof(contacts[count].email) - 1);
        contacts[count].email[sizeof(contacts[count].email) - 1] = '\0';
        
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = 0;
        strncpy(contacts[count].group, line, sizeof(contacts[count].group) - 1);
        contacts[count].group[sizeof(contacts[count].group) - 1] = '\0';
        
        count++;
    }
    
    fclose(file);

    file = fopen(CONTACTS_FILE, "w");
    if (file == NULL) {
        return 0;
    }
    
    int deleted = 0;
    for (int i = 0; i < count; i++) {
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

static void load_contacts(AppWidgets *widgets) {
    g_print("Starting to load contacts...\n");

    FILE *file = fopen(CONTACTS_FILE, "r");
    if (file == NULL) {
        g_print("Could not open contacts.txt file! Path: %s\n", CONTACTS_FILE);
        return;
    }

    g_print("File opened successfully.\n");

    int count = 0;
    char name[50], contact[15], email[50], group[20];
    char line[256]; 
    
    while (!feof(file)) {
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = 0;
        strncpy(name, line, sizeof(name) - 1);
        name[sizeof(name) - 1] = '\0';
        
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = 0;
        strncpy(contact, line, sizeof(contact) - 1);
        contact[sizeof(contact) - 1] = '\0';
        
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = 0;
        strncpy(email, line, sizeof(email) - 1);
        email[sizeof(email) - 1] = '\0';
        
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = 0;
        strncpy(group, line, sizeof(group) - 1);
        group[sizeof(group) - 1] = '\0';

        g_print("Loading contact %d: %s, %s, %s, %s\n", count+1, name, contact, email, group);
        
        add_contact_to_list(widgets, name, contact, email, group);
        count++;
    }
    
    fclose(file);
    g_print("Loaded %d contacts total.\n", count);

    gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(widgets->list_store), 0, GTK_SORT_ASCENDING);
}

static void clear_entries(AppWidgets *widgets) {
    gtk_editable_set_text(GTK_EDITABLE(widgets->name_entry), "");
    gtk_editable_set_text(GTK_EDITABLE(widgets->phone_entry), "");
    gtk_editable_set_text(GTK_EDITABLE(widgets->email_entry), "");
    gtk_editable_set_text(GTK_EDITABLE(widgets->group_entry), "");
}

static void add_clicked(GtkWidget *button, AppWidgets *widgets) {
    const char *name = gtk_editable_get_text(GTK_EDITABLE(widgets->name_entry));
    const char *phone = gtk_editable_get_text(GTK_EDITABLE(widgets->phone_entry));
    const char *email = gtk_editable_get_text(GTK_EDITABLE(widgets->email_entry));
    const char *group = gtk_editable_get_text(GTK_EDITABLE(widgets->group_entry));

    if (strlen(name) > 0 && strlen(phone) > 0) {
        add_contact_to_list(widgets, name, phone, email, group);
        save_contact_to_file(name, phone, email, group);
        clear_entries(widgets);
        g_print("Contact added successfully: %s\n", name);

        gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(widgets->list_store), 0, GTK_SORT_ASCENDING);
    }
}

static void search_changed(GtkSearchEntry *entry, AppWidgets *widgets) {
    const char *search_text = gtk_editable_get_text(GTK_EDITABLE(entry));
    GtkTreeModelFilter *filter = GTK_TREE_MODEL_FILTER(
        gtk_tree_view_get_model(GTK_TREE_VIEW(widgets->contact_list))
    );
    gtk_tree_model_filter_refilter(filter);
}


static void group_filter_changed(GtkComboBox *combo, AppWidgets *widgets) {
    GtkTreeModelFilter *filter = GTK_TREE_MODEL_FILTER(
        gtk_tree_view_get_model(GTK_TREE_VIEW(widgets->contact_list))
    );
    gtk_tree_model_filter_refilter(filter);
}

static gboolean filter_contacts(GtkTreeModel *model, GtkTreeIter *iter, gpointer data) {
    AppWidgets *widgets = (AppWidgets *)data;
    const char *search_text = gtk_editable_get_text(GTK_EDITABLE(widgets->search_entry));

    GtkTreeIter combo_iter;
    gchar *selected_group = NULL;
    if (gtk_combo_box_get_active_iter(GTK_COMBO_BOX(widgets->group_filter), &combo_iter)) {
        gtk_tree_model_get(gtk_combo_box_get_model(GTK_COMBO_BOX(widgets->group_filter)), 
                          &combo_iter, 0, &selected_group, -1);
    }
    
    char *name = NULL, *phone = NULL, *email = NULL, *group = NULL;
    gtk_tree_model_get(model, iter, 0, &name, 1, &phone, 2, &email, 3, &group, -1);
    
    gboolean visible = FALSE;
    if (name && phone && email && group) {
        gboolean group_match = TRUE;
        if (selected_group && strcmp(selected_group, "All Groups") != 0) {
            group_match = (strcasecmp(group, selected_group) == 0);
        }

        gboolean search_match = TRUE;
        if (search_text && strlen(search_text) > 0) {
            search_match = (strcasestr(name, search_text) ||
                          strcasestr(phone, search_text) ||
                          strcasestr(email, search_text) ||
                          strcasestr(group, search_text));
        }
        
        visible = group_match && search_match;
    }
    
    g_free(name); g_free(phone); g_free(email); g_free(group);
    g_free(selected_group);
    
    return visible;
}

static void delete_clicked(GtkWidget *button, AppWidgets *widgets) {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widgets->contact_list));
    GtkTreeModel *model;
    GtkTreeIter iter;
    
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        gchar *name = NULL, *contact = NULL;

        gtk_tree_model_get(model, &iter, 0, &name, 1, &contact, -1);
        
        if (name && contact) {
            DeleteDialogData *data = g_new(DeleteDialogData, 1);
            data->name = g_strdup(name);
            data->contact = g_strdup(contact);
            data->widgets = widgets;

            GtkWidget *dialog = gtk_dialog_new_with_buttons("Confirm Delete",
                                                          GTK_WINDOW(widgets->window),
                                                          GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                          "Yes",
                                                          GTK_RESPONSE_ACCEPT,
                                                          "No",
                                                          GTK_RESPONSE_CANCEL,
                                                          NULL);
                                                          
            GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
            char message[100];
            snprintf(message, sizeof(message), "Delete contact %s?", name);
            GtkWidget *label = gtk_label_new(message);
            gtk_widget_set_margin_start(label, 10);
            gtk_widget_set_margin_end(label, 10);
            gtk_widget_set_margin_top(label, 10);
            gtk_widget_set_margin_bottom(label, 10);
            
            gtk_box_append(GTK_BOX(content_area), label);
            gtk_widget_set_visible(label, TRUE);
            
            g_signal_connect(dialog, "response", G_CALLBACK(confirm_delete_response), data);
            gtk_window_present(GTK_WINDOW(dialog));
        }
        
        g_free(name);
        g_free(contact);
    } else {
        GtkWidget *dialog = gtk_dialog_new_with_buttons("Warning",
                                                      GTK_WINDOW(widgets->window),
                                                      GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                      "OK",
                                                      GTK_RESPONSE_ACCEPT,
                                                      NULL);
                                                      
        GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
        GtkWidget *label = gtk_label_new("Please select a contact to delete.");
        gtk_widget_set_margin_start(label, 10);
        gtk_widget_set_margin_end(label, 10);
        gtk_widget_set_margin_top(label, 10);
        gtk_widget_set_margin_bottom(label, 10);
        
        gtk_box_append(GTK_BOX(content_area), label);
        gtk_widget_set_visible(label, TRUE);
        
        g_signal_connect(dialog, "response", G_CALLBACK(warning_dialog_response), NULL);
        gtk_window_present(GTK_WINDOW(dialog));
    }
}

static void activate(GtkApplication *app, gpointer user_data) {
    AppWidgets *widgets = g_new0(AppWidgets, 1);

    widgets->window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(widgets->window), "Contact Manager");
    gtk_window_set_default_size(GTK_WINDOW(widgets->window), 800, 600);

    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_start(main_box, 10);
    gtk_widget_set_margin_end(main_box, 10);
    gtk_widget_set_margin_top(main_box, 10);
    gtk_widget_set_margin_bottom(main_box, 10);

    GtkWidget *filter_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);

    widgets->search_entry = gtk_search_entry_new();
    gtk_widget_set_hexpand(widgets->search_entry, TRUE);
    g_signal_connect(widgets->search_entry, "search-changed", G_CALLBACK(search_changed), widgets);

    widgets->group_store = gtk_list_store_new(1, G_TYPE_STRING);
    widgets->group_filter = gtk_combo_box_new_with_model(GTK_TREE_MODEL(widgets->group_store));
    
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(widgets->group_filter), renderer, TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(widgets->group_filter), renderer, "text", 0, NULL);
    
    g_signal_connect(widgets->group_filter, "changed", G_CALLBACK(group_filter_changed), widgets);

    gtk_box_append(GTK_BOX(filter_box), gtk_label_new("Filter by Group:"));
    gtk_box_append(GTK_BOX(filter_box), widgets->group_filter);
    gtk_box_append(GTK_BOX(filter_box), gtk_label_new("Search:"));
    gtk_box_append(GTK_BOX(filter_box), widgets->search_entry);
    
    gtk_widget_set_margin_bottom(filter_box, 10);

    widgets->list_store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

    gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(widgets->list_store), 0, sort_by_name, NULL, NULL);
    
    GtkTreeModelFilter *filter = GTK_TREE_MODEL_FILTER(gtk_tree_model_filter_new(GTK_TREE_MODEL(widgets->list_store), NULL));
    gtk_tree_model_filter_set_visible_func(filter, filter_contacts, widgets, NULL);

    widgets->contact_list = gtk_tree_view_new_with_model(GTK_TREE_MODEL(filter));
    
    const char *titles[] = {"Name", "Phone", "Email", "Group"};
    for (int i = 0; i < 4; i++) {
        GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(
            titles[i], gtk_cell_renderer_text_new(), "text", i, NULL
        );
        gtk_tree_view_append_column(GTK_TREE_VIEW(widgets->contact_list), column);
        
        gtk_tree_view_column_set_clickable(column, TRUE);
        gtk_tree_view_column_set_sort_column_id(column, i);
    }

    GtkWidget *scrolled = gtk_scrolled_window_new();
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrolled), 300);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled), widgets->contact_list);

    GtkWidget *form_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    widgets->name_entry = gtk_entry_new();
    widgets->phone_entry = gtk_entry_new();
    widgets->email_entry = gtk_entry_new();
    widgets->group_entry = gtk_entry_new();

    gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->name_entry), "Name");
    gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->phone_entry), "Phone");
    gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->email_entry), "Email");
    gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->group_entry), "Group");

    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_margin_top(button_box, 10);

    GtkWidget *add_button = gtk_button_new_with_label("Add Contact");
    g_signal_connect(add_button, "clicked", G_CALLBACK(add_clicked), widgets);

    widgets->delete_button = gtk_button_new_with_label("Delete Contact");
    g_signal_connect(widgets->delete_button, "clicked", G_CALLBACK(delete_clicked), widgets);

    gtk_box_append(GTK_BOX(button_box), add_button);
    gtk_box_append(GTK_BOX(button_box), widgets->delete_button);

    gtk_box_append(GTK_BOX(main_box), filter_box);
    gtk_box_append(GTK_BOX(main_box), scrolled);
    gtk_box_append(GTK_BOX(form_box), widgets->name_entry);
    gtk_box_append(GTK_BOX(form_box), widgets->phone_entry);
    gtk_box_append(GTK_BOX(form_box), widgets->email_entry);
    gtk_box_append(GTK_BOX(form_box), widgets->group_entry);
    gtk_box_append(GTK_BOX(form_box), button_box);
    gtk_box_append(GTK_BOX(main_box), form_box);

    gtk_window_set_child(GTK_WINDOW(widgets->window), main_box);

    gtk_window_present(GTK_WINDOW(widgets->window));

    load_groups_for_filter(widgets);
    load_contacts(widgets);

    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->group_filter), 0);
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

    Contact contacts[MAX_CONTACTS]; 
    int count = 0;

    char line[256];
    while (!feof(file) && count < MAX_CONTACTS) {
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = '\0';
        strncpy(contacts[count].name, line, sizeof(contacts[count].name) - 1);
        contacts[count].name[sizeof(contacts[count].name) - 1] = '\0';
        
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = '\0';
        strncpy(contacts[count].contact, line, sizeof(contacts[count].contact) - 1);
        contacts[count].contact[sizeof(contacts[count].contact) - 1] = '\0';
        
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = '\0';
        strncpy(contacts[count].email, line, sizeof(contacts[count].email) - 1);
        contacts[count].email[sizeof(contacts[count].email) - 1] = '\0';
        
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = '\0';
        strncpy(contacts[count].group, line, sizeof(contacts[count].group) - 1);
        contacts[count].group[sizeof(contacts[count].group) - 1] = '\0';
        
        count++;
    }
    
    fclose(file);

    qsort(contacts, count, sizeof(Contact), compare_contacts);

    printf("----- Contact List (%d contacts) -----\n", count);
    for (int i = 0; i < count; i++) {
        printf("%d. Name: %s\n   Contact: %s\n   Email: %s\n   Group: %s\n\n", 
               i + 1, contacts[i].name, contacts[i].contact, contacts[i].email, contacts[i].group);
    }
}


void terminal_show_by_group() {
    char groups[MAX_CONTACTS][20];
    int group_count = 0;
    get_unique_groups(groups, &group_count);

    printf("Available groups:\n");
    for (int i = 0; i < group_count; i++) {
        printf("%d. %s\n", i + 1, groups[i]);
    }

    int choice;
    printf("\nEnter group number (1-%d): ", group_count);
    if (scanf("%d", &choice) != 1 || choice < 1 || choice > group_count) {
        printf("Invalid choice!\n");
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        return;
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    char selected_group[20];
    strcpy(selected_group, groups[choice - 1]);
    

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
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = '\0';
        strncpy(name, line, sizeof(name) - 1);
        name[sizeof(name) - 1] = '\0';
        
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = '\0';
        strncpy(contact, line, sizeof(contact) - 1);
        contact[sizeof(contact) - 1] = '\0';
        
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = '\0';
        strncpy(email, line, sizeof(email) - 1);
        email[sizeof(email) - 1] = '\0';
        
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = '\0';
        strncpy(group, line, sizeof(group) - 1);
        group[sizeof(group) - 1] = '\0';

        if (strcasecmp(group, selected_group) == 0) {
            strcpy(filtered_contacts[filtered_count].name, name);
            strcpy(filtered_contacts[filtered_count].contact, contact);
            strcpy(filtered_contacts[filtered_count].email, email);
            strcpy(filtered_contacts[filtered_count].group, group);
            filtered_count++;
        }
    }
    
    fclose(file);

    qsort(filtered_contacts, filtered_count, sizeof(Contact), compare_contacts);

    printf("\n----- Contacts in group \"%s\" (%d contacts) -----\n", selected_group, filtered_count);
    for (int i = 0; i < filtered_count; i++) {
        printf("%d. Name: %s\n   Contact: %s\n   Email: %s\n   Group: %s\n\n", 
               i + 1, filtered_contacts[i].name, filtered_contacts[i].contact, 
               filtered_contacts[i].email, filtered_contacts[i].group);
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

    Contact results[MAX_CONTACTS];
    int found = 0;

    char line[256];
    char name[50], contact[15], email[50], group[20];
    
    while (!feof(file) && found < MAX_CONTACTS) {
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = '\0';
        strncpy(name, line, sizeof(name) - 1);
        name[sizeof(name) - 1] = '\0';
        
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = '\0';
        strncpy(contact, line, sizeof(contact) - 1);
        contact[sizeof(contact) - 1] = '\0';
        
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = '\0';
        strncpy(email, line, sizeof(email) - 1);
        email[sizeof(email) - 1] = '\0';
        
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

    qsort(results, found, sizeof(Contact), compare_contacts);

    printf("\n--- Search Results (%d contacts) ---\n", found);
    for (int i = 0; i < found; i++) {
        printf("%d. Name: %s\n   Contact: %s\n   Email: %s\n   Group: %s\n\n",
               i + 1, results[i].name, results[i].contact, results[i].email, results[i].group);
    }
}


void terminal_delete_contact() {
    FILE *file = fopen(CONTACTS_FILE, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open contacts file.\n");
        return;
    }

    Contact contacts[MAX_CONTACTS];
    int count = 0;

    char line[256];
    while (!feof(file) && count < MAX_CONTACTS) {
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = '\0';
        strncpy(contacts[count].name, line, sizeof(contacts[count].name) - 1);
        contacts[count].name[sizeof(contacts[count].name) - 1] = '\0';
        
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = '\0';
        strncpy(contacts[count].contact, line, sizeof(contacts[count].contact) - 1);
        contacts[count].contact[sizeof(contacts[count].contact) - 1] = '\0';
        
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = '\0';
        strncpy(contacts[count].email, line, sizeof(contacts[count].email) - 1);
        contacts[count].email[sizeof(contacts[count].email) - 1] = '\0';
        
        if (fgets(line, sizeof(line), file) == NULL) break;
        line[strcspn(line, "\n")] = '\0';
        strncpy(contacts[count].group, line, sizeof(contacts[count].group) - 1);
        contacts[count].group[sizeof(contacts[count].group) - 1] = '\0';
        
        count++;
    }

    fclose(file);

    qsort(contacts, count, sizeof(Contact), compare_contacts);

    printf("----- Contact List (%d contacts) -----\n", count);
    for (int i = 0; i < count; i++) {
        printf("%d. Name: %s\n   Contact: %s\n   Email: %s\n   Group: %s\n\n", 
               i + 1, contacts[i].name, contacts[i].contact, contacts[i].email, contacts[i].group);
    }

    int choice;
    printf("Enter number of the contact to delete (1-%d) or 0 to cancel: ", count);
    if (scanf("%d", &choice) != 1 || choice < 0 || choice > count) {
        printf("Invalid choice!\n");
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        return;
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    
    if (choice == 0) {
        printf("Deletion cancelled.\n");
        return;
    }

    char confirm;
    printf("Are you sure you want to delete %s? (y/n): ", contacts[choice-1].name);
    scanf("%c", &confirm);

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


static void confirm_delete_response(GtkDialog *dialog, int response, gpointer user_data) {
    DeleteDialogData *data = (DeleteDialogData *)user_data;
    
    if (response == GTK_RESPONSE_ACCEPT) {
        if (delete_contact_from_file(data->name, data->contact)) {
            g_print("Contact deleted from file: %s\n", data->name);
            
            gtk_list_store_clear(data->widgets->list_store);
            load_contacts(data->widgets);
            
            gtk_list_store_clear(data->widgets->group_store);
            load_groups_for_filter(data->widgets);
            gtk_combo_box_set_active(GTK_COMBO_BOX(data->widgets->group_filter), 0);
            
            GtkTreeModelFilter *filter = GTK_TREE_MODEL_FILTER(
                gtk_tree_view_get_model(GTK_TREE_VIEW(data->widgets->contact_list))
            );
            gtk_tree_model_filter_refilter(filter);
        } else {
            GtkWidget *error_dialog = gtk_dialog_new_with_buttons("Error",
                                                GTK_WINDOW(data->widgets->window),
                                                GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                "OK",
                                                GTK_RESPONSE_ACCEPT,
                                                NULL);
                                                
            GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(error_dialog));
            GtkWidget *label = gtk_label_new("Failed to delete contact.");
            gtk_widget_set_margin_start(label, 10);
            gtk_widget_set_margin_end(label, 10);
            gtk_widget_set_margin_top(label, 10);
            gtk_widget_set_margin_bottom(label, 10);
            
            gtk_box_append(GTK_BOX(content_area), label);
            gtk_widget_set_visible(label, TRUE);
            
            g_signal_connect(error_dialog, "response", G_CALLBACK(error_dialog_response), NULL);
            gtk_window_present(GTK_WINDOW(error_dialog));
        }
    }
    
    g_free(data->name);
    g_free(data->contact);
    g_free(data);
    
    gtk_window_destroy(GTK_WINDOW(dialog));
}

static void error_dialog_response(GtkDialog *dialog, int response, gpointer user_data) {
    gtk_window_destroy(GTK_WINDOW(dialog));
}

static void warning_dialog_response(GtkDialog *dialog, int response, gpointer user_data) {
    gtk_window_destroy(GTK_WINDOW(dialog));
}

void terminal_mode_main() {
    int choice;

    while (1) {
        printf("\n====== Contact Manager (Terminal Mode) ======\n");
        printf("1. Add Contact\n");
        printf("2. Show All Contacts\n");
        printf("3. Search Contact\n");
        printf("4. Show Contacts by Group\n");
        printf("5. Delete Contact\n");  
        printf("6. Exit\n");
        printf("Enter choice: ");
        
        if (scanf("%d", &choice) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            printf("Invalid input. Please enter a number.\n");
            continue;
        }

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
                return;
            default:
                printf("Invalid choice! Try again.\n");
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1 && (strcmp(argv[1], "--terminal") == 0 || strcmp(argv[1], "-t") == 0)) {
        terminal_mode_main();
        return 0;
    }

    GtkApplication *app = gtk_application_new("org.gtk.contactmanager", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}