#include <gtk/gtk.h>
#include "file_select.h"
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
static void on_click(GtkWidget *button, gpointer data){
    GtkWidget *dialog = gtk_window_new();
    gtk_window_set_child(GTK_WINDOW(dialog),file_select_window(NULL,g_file_new_for_path(get_executable_folder())));
    //gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(dialog),GTK_WINDOW(gtk_widget_get_root(button)));
    GdkRectangle moniter = GetMonitorSize(GTK_WIDGET(gtk_widget_get_root(button)));
    gtk_window_present(GTK_WINDOW(dialog));
    gtk_window_set_default_size(GTK_WINDOW(dialog),0.5*moniter.width,0.5*moniter.height);
}
static void
on_activate(GtkApplication *app, gpointer data)
{
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_present(GTK_WINDOW(window));
    GdkRectangle moniter = GetMonitorSize(window);
    GtkWidget *button = gtk_button_new_with_label("test");
    
    g_signal_connect(GTK_BUTTON(button),"clicked",G_CALLBACK(on_click),NULL);
    gtk_window_set_child(GTK_WINDOW(window),button);
    gtk_window_maximize(GTK_WINDOW(window));
}

int main(int argc, char **argv)
{
    GtkApplication *app;

    app = gtk_application_new("com.example.filebrowser",
                              G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);

    int status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);

    return status;
}