#include "get_executable_folder.h"
#include <gtk/gtk.h>
/* -------- SETUP: create UI -------- */
void setup_cb(GtkSignalListItemFactory *factory,
              GtkListItem *list_item,
              gpointer user_data)
{
    GtkWidget *button = gtk_button_new();
    gtk_list_item_set_child(list_item, button);
}

/* -------- BIND: assign data -------- */
void bind_cb(GtkSignalListItemFactory *factory,
             GtkListItem *list_item,
             gpointer user_data)
{
    GtkWidget *button = gtk_list_item_get_child(list_item);
    GFileInfo *info = gtk_list_item_get_item(list_item);
    const char *text = g_file_info_get_name(info);
    printf("\n%s\n",g_file_info_get_name(info));

    gtk_button_set_label(GTK_BUTTON(button), text);
}
GtkWidget* file_select_window(GtkWidget *button,GFile *path){
    printf("%s",get_executable_folder());
    // GFile *path = g_file_new_for_path(path);
    GtkDirectoryList *dirlist = gtk_directory_list_new("standard::*",path);
    GtkListItemFactory *factory = gtk_signal_list_item_factory_new();
    g_signal_connect(factory,"setup",G_CALLBACK(setup_cb),NULL);
    g_signal_connect(factory,"bind",G_CALLBACK(bind_cb),NULL);
    GtkWidget *list = gtk_grid_view_new(GTK_SELECTION_MODEL(gtk_single_selection_new(G_LIST_MODEL(dirlist))),GTK_LIST_ITEM_FACTORY(factory));
    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll),list);
    return scroll;
}