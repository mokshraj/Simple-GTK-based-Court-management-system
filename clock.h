#ifndef CLOCK__H
#define CLOCK__H
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h> // For va_list in execute_sql_update
#include <time.h>   // For time() and localtime()
#include <sqlite3.h> // For SQLite functions
#include <gtk/gtk.h> // For GTK types and functions
typedef struct {
    guint seconds;         // main timer
    guint break_seconds;   // break timer
    gboolean running;      // is main timer running?
    gboolean paused;       // is it paused (break mode)?
    guint timer_id;        // timeout callback ID
    GtkLabel *label;       // main timer label
    GtkLabel *break_label; // break timer label
    
    // --- UPDATED FIELDS FOR DB CONTEXT ---
    int *schedule_id;       // The ID of the schedule record to update
    // ---------------------------------
} TimerData;
// Function definitions for utility functions
static void format_time_hms(guint total_seconds, char *buffer, size_t size);
static void get_current_time_str(char *buffer, size_t size);
static int execute_sql_update(TimerData *data, const char *sql_format, ...);


// Function to convert seconds to HH:MM:SS format
static void format_time_hms(guint total_seconds, char *buffer, size_t size) {
    guint hrs  = total_seconds / 3600;
    guint mins = (total_seconds % 3600) / 60;
    guint secs = total_seconds % 60;
    snprintf(buffer, size, "%02u:%02u:%02u", hrs, mins, secs);
}

// Function to get the current system time as HH:MM:SS string
static void get_current_time_str(char *buffer, size_t size) {
    time_t timer;
    struct tm *tm_info;
    time(&timer);
    tm_info = localtime(&timer);
    strftime(buffer, size, "%H:%M:%S", tm_info);
}

// Function to execute a simple SQL update statement
static int execute_sql_update(TimerData *data, const char *sql_format, ...) {
    sqlite3 *db;
    char *err_msg = NULL;
    // Database is opened directly here
    int rc = sqlite3_open("Judgment.db", &db);
    int success = 0;

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        // Note: db is NULL or not properly initialized if open fails
        if (db) sqlite3_close(db); 
        return 0;
    }

    // Use varargs to construct the dynamic SQL string
    va_list args;
    va_start(args, sql_format);
    char *sql = g_strdup_vprintf(sql_format, args);
    va_end(args);

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error in schedule update (ID: %d): %s\n", *(data->schedule_id), err_msg);
        sqlite3_free(err_msg);
    } else {
        success = 1;
        g_print("Schedule ID %d updated successfully with SQL: %s\n", *(data->schedule_id), sql);
    }
    
    g_free(sql);
    sqlite3_close(db); // Database is closed before returning
    return success;
}

static gboolean update_time (gpointer user_data)
{
    TimerData *data = (TimerData *)user_data;
    
    if (data->running) {
        data->seconds++;
    } else if (data->paused) {
        data->break_seconds++;
    } else {
        return G_SOURCE_CONTINUE;
    }

    // Update display: main timer
    char buffer[32];
    format_time_hms(data->seconds, buffer, sizeof(buffer));
    gtk_label_set_text(data->label, buffer);

    // ----------------------------------------------------------------
    // FIX APPLIED HERE: Using two distinct buffers (time_str and label_buffer)
    // to prevent memory corruption when formatting the string.
    char time_str[16]; // Holds only the time: HH:MM:SS
    char label_buffer[32]; // Holds the final label: "Break: HH:MM:SS"

    // 1. Format the time into the temporary buffer
    format_time_hms(data->break_seconds, time_str, sizeof(time_str));

    // 2. Safely create the final label string
    snprintf(label_buffer, sizeof(label_buffer), "Break: %s", time_str);
    
    gtk_label_set_text(data->break_label, label_buffer);
    // ----------------------------------------------------------------

    return G_SOURCE_CONTINUE;
}


// --- on_play_clicked (No functional changes, logic remains the same) ---
static void on_play_clicked(GtkButton *button, gpointer user_data)
{
    TimerData *data = (TimerData *)user_data;
    
    if (data->running) return;

    data->running = TRUE;
    data->paused = FALSE;
    
    if (data->seconds == 0) {
        char current_time[9];
        get_current_time_str(current_time, sizeof(current_time));
        
        // SQL: Update the start_time when proceeding starts
        const char *sql_format = 
            "UPDATE case_schedule SET start_time = '%s', status = 'Ongoing', updated_at = CURRENT_TIMESTAMP WHERE schedule_id = %d;";

        execute_sql_update(data, sql_format, current_time, *(data->schedule_id));
    }
}

// --- on_pause_clicked (No changes needed) ---
static void on_pause_clicked(GtkButton *button, gpointer user_data)
{
    TimerData *data = (TimerData *)user_data;
    if (data->running) {
        data->running = FALSE;
        data->paused = TRUE;
    }
}

// --- on_reset_clicked (No functional changes, logic remains the same) ---
static void on_reset_clicked(GtkButton *button, gpointer user_data)
{
    TimerData *data = (TimerData *)user_data;
    
    if (data->seconds > 0 || data->break_seconds > 0) {
        
        char end_time[9];
        char proceeding_time_str[9];
        char break_time_str[9];
        
        get_current_time_str(end_time, sizeof(end_time));
        format_time_hms(data->seconds, proceeding_time_str, sizeof(proceeding_time_str));
        format_time_hms(data->break_seconds, break_time_str, sizeof(break_time_str));

        // SQL: Update end_time, Proceding_time, Break_time, and status
        const char *sql_format = 
            "UPDATE case_schedule SET "
            "end_time = '%s', "
            "Proceding_time = '%s', "
            "Break_time = '%s', "
            "status = 'Completed', "
            "updated_at = CURRENT_TIMESTAMP "
            "WHERE schedule_id = %d;";
        
        execute_sql_update(data, sql_format, 
                           end_time, 
                           proceeding_time_str, 
                           break_time_str, 
                           *(data->schedule_id));
    }
    
    // Reset local timer state
    data->running = FALSE;
    data->paused  = FALSE;
    data->seconds = 0;
    data->break_seconds = 0;

    gtk_label_set_text(data->label, "00:00:00");
    gtk_label_set_text(data->break_label, "Break: 00:00:00");
}


// --- clock_ (Simplified signature) ---
GtkWidget *clock_(int *schedule_id)
{
    GtkWidget *Case_control,*play_btn, *pause_btn, *reset_btn,*label,*break_label;

    TimerData *timer_data = g_new0(TimerData, 1);
    
    // --- Store DB Context ---
    timer_data->schedule_id = schedule_id;
    // ------------------------

    Case_control = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_top(Case_control, 15);
    gtk_widget_set_margin_bottom(Case_control, 15);
    gtk_widget_set_margin_start(Case_control, 15);
    gtk_widget_set_margin_end(Case_control, 15);

    // Main timer label
    label = gtk_label_new("00:00:00");
    gtk_widget_add_css_class(label, "special");
    timer_data->label = GTK_LABEL(label);
    gtk_box_append(GTK_BOX(Case_control), label);

    // Break timer label
    break_label = gtk_label_new("Break: 00:00:00");
    gtk_widget_add_css_class(break_label, "special");
    timer_data->break_label = GTK_LABEL(break_label);
    gtk_box_append(GTK_BOX(Case_control), break_label);

    // Play-Pause-Reset buttons
    GtkWidget *btn_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_set_homogeneous(GTK_BOX(btn_box),TRUE);
    gtk_box_append(GTK_BOX(Case_control), btn_box);

    play_btn  = gtk_button_new_with_label("Start Proceding");
    pause_btn = gtk_button_new_with_label("Pause Proceding");
    reset_btn = gtk_button_new_with_label("End Proceding");

    gtk_box_append(GTK_BOX(btn_box), play_btn);
    gtk_box_append(GTK_BOX(btn_box), pause_btn);
    gtk_box_append(GTK_BOX(btn_box), reset_btn);

    g_signal_connect(play_btn,  "clicked", G_CALLBACK(on_play_clicked),  timer_data);
    g_signal_connect(pause_btn, "clicked", G_CALLBACK(on_pause_clicked), timer_data);
    g_signal_connect(reset_btn, "clicked", G_CALLBACK(on_reset_clicked), timer_data);
    // 1s interval update for timer
    timer_data->timer_id = g_timeout_add_seconds(1, update_time, timer_data);

    // Add a destroy notify to clean up timer_data when Case_control is destroyed
    g_object_set_data_full(G_OBJECT(Case_control), "timer_data", timer_data, g_free);
    gtk_widget_set_sensitive(Case_control, FALSE);
    return Case_control;
}
#endif