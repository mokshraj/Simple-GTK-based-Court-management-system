#include <gtk/gtk.h>
#include <sqlite3.h>
GdkRectangle GetMonitorSize(GtkWidget *window);
GtkWidget *Schedule_list(int *set_id,GDateTime* Date_time);
void activate(GtkApplication *app, gpointer user_data);
static void Future_schedule_panel(GtkWidget *widget,gpointer data);
static void set_schedule(GtkWidget *widget,gpointer data);
GdkRectangle GetMonitorSize(GtkWidget *window){
    // Get the surface for the window
    GdkSurface *surface = gtk_native_get_surface(GTK_NATIVE(window));
    if (surface == NULL) {
        g_printerr("Couldn't get native surface.\n");
        exit(-1);
    }
    
    // Get the default display
    GdkDisplay *display = gdk_display_get_default();
    if (display == NULL) {
        g_printerr("No default display.\n");
        exit(-1);
    }
    
    // Get the monitor for that surface
    GdkMonitor *monitor = gdk_display_get_monitor_at_surface(display, surface);
    if (monitor == NULL) {
        g_printerr("Could not determine monitor for window surface.\n");
        exit(-1);
    }
    
    // Get and return monitor geometry
    GdkRectangle geometry;
    gdk_monitor_get_geometry(monitor, &geometry);
    return geometry;
}
GtkWidget *Schedule_list(int *set_id,GDateTime* Date_time){
    char * Date;
    if (Date_time == NULL){
        Date = g_strdup_printf("CURRENT_DATE");
    }
    else{
        Date = g_strdup_printf("'%d-%02d-%02d'",g_date_time_get_year(Date_time),g_date_time_get_month(Date_time),g_date_time_get_day_of_month(Date_time));
        g_print(Date);
        g_date_time_unref(Date_time);
    }
    GtkWidget *button,*Schedule_list;
    Schedule_list = gtk_scrolled_window_new();
    gtk_widget_add_css_class(Schedule_list,"scroll");
    gtk_widget_set_vexpand(Schedule_list, TRUE);  // Allow vertical expansion
    gtk_widget_set_hexpand(Schedule_list, TRUE);  // Allow horizontal expansion
    GtkWidget *list_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);  // Container for labels
    // Add labels to the box
    sqlite3 *db;
    sqlite3_stmt *stmt;
    char *err_msg = NULL;
    int rc = sqlite3_open("Judgment.db",&db);
        if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }
    char *sql;
        sql = g_strdup_printf(
            "SELECT cs.case_id, ct.Name, cs.schedule_id, cs.start_time, cs.status "
            "FROM case_schedule cs "
            "INNER JOIN Crime_table ct ON cs.case_id = ct.Case_ID "
            "WHERE cs.date = %s;",
            Date
        );
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int case_id = sqlite3_column_int(stmt, 0);
        const unsigned char *name = sqlite3_column_text(stmt, 1);
        int schedule_id = sqlite3_column_int(stmt,2);
        const unsigned char *time = sqlite3_column_text(stmt, 3);
        const unsigned char *status = sqlite3_column_text(stmt, 4);
        button =  gtk_button_new_with_label(g_strdup_printf("Case ID : %d, %s , S.T : %s %s",case_id,name,time,status));
        g_object_set_data_full(G_OBJECT(button),"Schedule_id",g_memdup2(&(int){schedule_id},sizeof(int)),g_free);
        // g_signal_connect(button,"clicked",G_CALLBACK(set_),g_memdup2(&(widget_and_id){casecontrol,set_id},sizeof(widget_and_id)));
        gtk_box_append(GTK_BOX(list_box),button);
    }
    g_free(Date);
    g_free(sql);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    // Set the box as the scrolled window's child
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(Schedule_list), list_box);
    gtk_scrolled_window_set_has_frame(GTK_SCROLLED_WINDOW(Schedule_list),TRUE);
    return Schedule_list;
}
void activate(GtkApplication *app, gpointer user_data){
    //Login data
    // Load CSS styling
    GtkCssProvider *provider = gtk_css_provider_new(); 
    gtk_css_provider_load_from_path(provider, "style.css");
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER
    );
    g_object_unref(provider);
    //
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Schedule-Box");
    gtk_window_present(GTK_WINDOW(window));
    GdkRectangle geometry = GetMonitorSize(window);
    gtk_window_set_resizable(GTK_WINDOW(window), TRUE);
    gtk_window_maximize(GTK_WINDOW(window));
    // Schedule
    int *schedule_id = g_new0(int, 1);
    GtkWidget *box = gtk_grid_new();
    GtkWidget *icon1 = gtk_image_new_from_file("schedule-time-and-date-svgrepo-com.svg");
    GtkWidget *label1 = gtk_label_new("schedule");
    gtk_widget_add_css_class(label1,"label");
    gtk_widget_add_css_class(icon1,"label");
    gtk_image_set_pixel_size(GTK_IMAGE(icon1),geometry.width * 0.02);
    gtk_grid_attach(GTK_GRID(box),icon1,0,0,1,1);
    gtk_grid_attach(GTK_GRID(box),label1,1,0,1,1);
    gtk_grid_attach(GTK_GRID(box),Schedule_list(schedule_id,NULL),0,1,2,1);
    GtkWidget *button = gtk_button_new_with_label("Open Future Schedules");
    GdkRectangle *copy = g_new(GdkRectangle, 1); // allocates memory
    *copy = geometry;                            // copy struct contents
    g_object_set_data_full(G_OBJECT(button), "geometry", copy, g_free);
    g_object_set_data(G_OBJECT(button),"schedule_id",schedule_id);
    g_signal_connect(button,"clicked",G_CALLBACK(Future_schedule_panel),NULL);
    gtk_grid_attach(GTK_GRID(box),button,0,2,2,1);
    gtk_window_set_child(GTK_WINDOW(window),box);
    
}
static void Future_schedule_panel(GtkWidget *widget,gpointer data){
    fflush(stdout);
    GtkWidget* window = gtk_window_new();
    GtkWidget *calendar = gtk_calendar_new();
    GdkRectangle geometry = *(GdkRectangle *)g_object_get_data(G_OBJECT(widget),"geometry");
    GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL,geometry.width*0.015);
    gtk_box_append(GTK_BOX(box),calendar);
    gtk_box_append(GTK_BOX(box),gtk_label_new("Select a date to proceed"));
    gtk_box_set_homogeneous(GTK_BOX(box),TRUE);
    gtk_window_set_child(GTK_WINDOW(window), box);
    g_object_set_data(G_OBJECT(calendar),"data_box",widget);
    g_signal_connect(calendar, "day-selected", G_CALLBACK(set_schedule), box);
    gtk_window_set_default_size(GTK_WINDOW(window),(int)(geometry.width * 0.3),(int)(geometry.height * 0.5));
    gtk_window_present(GTK_WINDOW(window));
}
static void set_schedule(GtkWidget *widget,gpointer data){
    GDateTime *date = gtk_calendar_get_date(GTK_CALENDAR(widget));
    g_print("\nSelected date: %d-%02d-%02d\n",
            g_date_time_get_year(date),
            g_date_time_get_month(date),
            g_date_time_get_day_of_month(date));
    gtk_widget_unparent(gtk_widget_get_last_child(GTK_WIDGET(data)));
    GtkWidget *data_box = (GtkWidget *)g_object_get_data(G_OBJECT(widget),"data_box");
    int *schedule_id = (int *)g_object_get_data(G_OBJECT(data_box),"schedule_id");
    gtk_box_append(GTK_BOX(data),Schedule_list(schedule_id,date));
}
int main(int argc, char **argv)
{
    GtkApplication *app;
    int status;
    
    // Create GTK application
    app = gtk_application_new("org.gtk.mokshraj", G_APPLICATION_DEFAULT_FLAGS);
    
    // Connect activation handler and run application
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    
    return status;
}