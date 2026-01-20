#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h> // For va_list in execute_sql_update
#include <time.h>   // For time() and localtime()

// Structure definitions
typedef struct widget_and_id {
    GtkWidget *widget;
    int *num;
} widget_and_id;

typedef struct p_pair {
    void **a;
    void *b;
} p_pair;

typedef struct login_data {
    GtkWidget *userid;    // Entry widget for user ID
    GtkWidget *pass;      // Entry widget for password
    int type;             // Login type (1 for Judge, 2 for Lawyer, 3 for Stenographer)
    char *user_id;
    char *pass_;
} login_data;

// Callback functions
static void switch_(GtkWidget *button, gpointer data);
void set(GtkWidget *widget, gpointer data);
void set_(GtkWidget *widget, gpointer data);

// Utility functions
void open_google_meet(void);
GdkRectangle GetMonitorSize(GtkWidget *window);
gboolean ensure_directory_exists(const char *path);
static void open_file(GtkWidget *button,gpointer relative);
static void gui_open_file(GtkWidget *button,gpointer relative);
static void open_python_file(GtkWidget *button,gpointer relative);

// Login and authentication
void Login(GtkButton *widget, gpointer user_data);

// Window callbacks
void on_window_realize(GtkWidget *window, gpointer user_data);
void activate(GtkApplication *app, gpointer user_data);

// Timer functions
static gboolean update_time(gpointer user_data);
static void on_play_clicked(GtkButton *button, gpointer user_data);
static void on_pause_clicked(GtkButton *button, gpointer user_data);
static void on_reset_clicked(GtkButton *button, gpointer user_data);

// Layout constraint functions
static void Schedule_grid_contraints(GtkLayoutManager *layout, GtkWidget *container,GtkWidget *box, GdkRectangle geometry);
static void CaseControlPanel_contraints(GtkLayoutManager *layout, GtkWidget *box,GtkWidget *CaseControlPanel, GdkRectangle geometry);
static void note_box_contraints(GtkLayoutManager *layout,GtkWidget*container,GtkWidget *list,GtkWidget*box,GdkRectangle geometry);
static void info_box_contraints(GtkLayoutManager *layout,GtkWidget*container,GtkWidget *notes_box,GtkWidget*info_box,GdkRectangle geometry);

// Widget creation functions
GtkWidget *CaseControlPanel(void);
GtkWidget *LawyerControlPanel(void);
GtkWidget *Schedule_list(login_data *login,int *set_id,GtkWidget *casecontrol,GDateTime* Date_time,GtkWidget **widgets);
GtkWidget* Textbox(int *schedule_id,char *file,login_data *login,char *label_name);
GtkWidget* Infobox(int *schedule_id,char *file,login_data *login);

// File dialog functions
static void dialog_open_file(GObject *source_object, GAsyncResult *res, gpointer user_data);
static void open_folder(GtkWidget *button, gpointer user_data);

// Dashboard functions
void Judge_dash(GtkWidget *window, gpointer user_data);
void Lawyer_dash(GtkWidget *window, gpointer user_data);
void Stenographer_dash(GtkWidget *window, gpointer user_data);
static void Future_schedule_panel(GtkWidget *widget,gpointer data);
static void set_schedule(GtkWidget *widget,gpointer data);

// Video hearing functions
static void update_video_id(GtkWidget *widget, gpointer data);
static void Video_hearing_start(GtkWidget *widget, gpointer data);
static void Video_hearing_open(GtkWidget *widget, gpointer data);

// TextView functions
static void on_save_clicked(GtkWidget *widget, gpointer user_data);
static void on_reload_clicked(GtkWidget *widget, gpointer user_data);

#endif // GUI_H