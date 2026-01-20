#ifndef schedule_h
#define schedule_h
#include <gtk/gtk.h>
GdkRectangle GetMonitorSize(GtkWidget *window);
GtkWidget *Schedule_list(int *set_id,GDateTime* Date_time);
void activate(GtkApplication *app, gpointer user_data);
static void Future_schedule_panel(GtkWidget *widget,gpointer data);
static void set_schedule(GtkWidget *widget,gpointer data);
#endif