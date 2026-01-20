#include "gui.c"
/* Main function */
int main(int argc, char **argv)
{
    creating_database();
    GtkApplication *app;
    int status;
    
    // Create GTK application
    app = gtk_application_new("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
    
    // Connect activation handler and run application
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}