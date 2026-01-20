#include <gtk/gtk.h>
#include <sqlite3.h>
#include "get_executable_folder.h"
// Callback for Save button
static void on_save_clicked(GtkWidget *widget, gpointer user_data) {
    int *schedule_id = (int *)g_object_get_data(G_OBJECT(widget),"schedule_id");
    sqlite3 *db;
    sqlite3_stmt *stmt;
    char *err_msg = NULL;
    int rc = sqlite3_open("Judgment.db",&db);
        if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
    char *sql;
    sql = g_strdup_printf("SELECT Case_id FROM case_schedule WHERE schedule_id = %d ;",*schedule_id);
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_step(stmt);
    int case_id = sqlite3_column_int(stmt, 0);
    g_print("%d",case_id);
    g_free(sql);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    //
    //
    char *file_name = g_strdup_printf("%s/Cases/%d/%s",get_executable_folder(),case_id,(char *)g_object_get_data(G_OBJECT(widget),"file"));
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(user_data));
    GtkTextIter start, end;
    gchar *text;

    gtk_text_buffer_get_bounds(buffer, &start, &end);
    text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

    FILE *file = fopen(file_name, "w");
    if (file) {
        fputs(text, file);
        fclose(file);
        g_print("Saved to output.txt\n");
    } else {
        g_print("Error: Could not save file.\n");
    }
    g_free(text);
}
static void on_reload_clicked(GtkWidget *widget, gpointer user_data){
    int *schedule_id = (int *)g_object_get_data(G_OBJECT(widget),"schedule_id");
    sqlite3 *db;
    sqlite3_stmt *stmt;
    char *err_msg = NULL;
    int rc = sqlite3_open("Judgment.db",&db);
        if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
    char *sql;
    sql = g_strdup_printf("SELECT Case_id FROM case_schedule WHERE schedule_id = %d ;",*schedule_id);
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_step(stmt);
    int case_id = sqlite3_column_int(stmt, 0);
    g_print("%d",case_id);
    g_free(sql);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    //
    //
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(user_data));
    gchar *contents = NULL;
    gsize length;
    //
    char *file = g_strdup_printf("%s/Cases/%d/%s",get_executable_folder(),case_id,(char *)g_object_get_data(G_OBJECT(widget),"file"));
    //
    if (g_file_get_contents(file, &contents, &length, NULL)) {
        gtk_text_buffer_set_text(buffer, contents, length);
        g_free(contents);
    } else {
        g_print("Error: Could not read file.\n");
    }
    g_free(file);
}
GtkWidget* Textbox(int *schedule_id){
    GtkWidget *box;
    GtkWidget *text_view;
    GtkWidget *button;
    GtkWidget *scroll;
    GtkWidget *buttonbox;
    GtkWidget *label;
    //
    
    //
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    label = gtk_label_new("Judges notes");
    gtk_widget_set_hexpand(label,TRUE);
    gtk_box_append(GTK_BOX(box), label);
    // Create vertical box layout
    buttonbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,5);
    // Create text view for user input
    text_view = gtk_text_view_new();
    scroll = gtk_scrolled_window_new();
    gtk_widget_set_vexpand(scroll, TRUE);  // Allow vertical expansion
    gtk_widget_set_hexpand(scroll, TRUE);  // Allow horizontal expansion
    gtk_widget_set_hexpand(text_view, TRUE);
    gtk_widget_set_vexpand(text_view, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll),text_view);
    gtk_scrolled_window_set_kinetic_scrolling(GTK_SCROLLED_WINDOW(scroll),TRUE);
    gtk_box_append(GTK_BOX(box), scroll);
    // Create save button
    button = gtk_button_new_with_label("Save Text");
    g_object_set_data(G_OBJECT(button),"schedule_id",schedule_id);
    g_object_set_data_full(G_OBJECT(button),"file",g_strdup_printf("Judge_notes.txt"),g_free);
    g_signal_connect(button, "clicked", G_CALLBACK(on_save_clicked), text_view);
    gtk_box_append(GTK_BOX(buttonbox),button);
    button = gtk_button_new_with_label("Reload");
    g_object_set_data_full(G_OBJECT(button),"file",g_strdup_printf("Judge_notes.txt"),g_free);
    g_object_set_data(G_OBJECT(button),"schedule_id",schedule_id);
    g_signal_connect(button, "clicked", G_CALLBACK(on_reload_clicked), text_view);
    gtk_box_append(GTK_BOX(buttonbox),button);
    gtk_box_append(GTK_BOX(box), buttonbox);
    // Connect signal
    return box;
}
GtkWidget* Infobox(int *schedule_id){
    GtkWidget *box;
    GtkWidget *text_view;
    GtkWidget *button;
    GtkWidget *scroll;
    GtkWidget *buttonbox;
    GtkWidget *label;
    // Create vertical box layout
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    label = gtk_label_new("Case Info");
    gtk_widget_set_hexpand(label,TRUE);
    gtk_box_append(GTK_BOX(box), label);
    buttonbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,5);
    // Create text view for user input
    text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view),FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(text_view), FALSE);
    scroll = gtk_scrolled_window_new();
    gtk_widget_set_hexpand(text_view, TRUE);
    gtk_widget_set_vexpand(text_view, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll),text_view);
    gtk_scrolled_window_set_kinetic_scrolling(GTK_SCROLLED_WINDOW(scroll),TRUE);
    gtk_box_append(GTK_BOX(box), scroll);
    button = gtk_button_new_with_label("Reload");
    g_object_set_data_full(G_OBJECT(button),"file",g_strdup_printf("Info.txt"),g_free);
    g_object_set_data(G_OBJECT(button),"schedule_id",schedule_id);
    g_signal_connect(button, "clicked", G_CALLBACK(on_reload_clicked), text_view);
    gtk_box_append(GTK_BOX(buttonbox),button);
    gtk_box_append(GTK_BOX(box), buttonbox);
    // Connect signal
    return box;
}
// Activate callback for GtkApplication
static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *box;
    int *schedule_id = g_new0(int,1);
    *schedule_id = 1;
    // Create main window
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Text Save Example");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

    // Create vertical box layout
    box = Infobox(schedule_id);
    gtk_window_set_child(GTK_WINDOW(window), box);

    gtk_window_present(GTK_WINDOW(window));
}

// Main function
int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.gtk.textsave", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
