#include "get_executable_folder.h"
#include <gtk/gtk.h>
void open_file_viewer(GFile *file)
{
    char *path = g_file_get_path(file);

    GFileInfo *info = g_file_query_info(
        file,
        "standard::content-type",
        G_FILE_QUERY_INFO_NONE,
        NULL,
        NULL
    );

    const char *type = g_file_info_get_content_type(info);

    GtkWidget *win = gtk_window_new();
    gtk_window_set_default_size(GTK_WINDOW(win), 600, 400);

    GtkWidget *child = NULL;

    /* ---------- TEXT ---------- */
    if (g_str_has_prefix(type, "text/")) {
        GtkWidget *view = gtk_text_view_new();
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));

        gchar *content = NULL;
        g_file_load_contents(file, NULL, &content, NULL, NULL, NULL);

        if (content) {
            gtk_text_buffer_set_text(buffer, content, -1);
            g_free(content);
        }

        child = view;
    }

    /* ---------- IMAGE ---------- */
    else if (g_str_has_prefix(type, "image/")) {
        child = gtk_image_new_from_file(path);
    }

    /* ---------- VIDEO ---------- */
    else if (g_str_has_prefix(type, "video/")) {
        child = gtk_label_new("Video not supported yet");
    }

    else {
        child = gtk_label_new("Unsupported file");
    }

    gtk_window_set_child(GTK_WINDOW(win), child);
    gtk_window_present(GTK_WINDOW(win));

    g_free(path);
    g_object_unref(info);
}
void on_file_clicked(GtkButton *button, gpointer user_data)
{
    const char *name =
        g_object_get_data(G_OBJECT(button), "file-name");

    GFile *dir =
        g_object_get_data(G_OBJECT(button), "dir");

    if (!name || !dir) return;

    GFile *file = g_file_get_child(dir, name);

    open_file_viewer(file);

    g_object_unref(file);
}
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
    GFile *dir = user_data;
    const char *name = g_file_info_get_name(info);
    gtk_button_set_label(GTK_BUTTON(button), name);
    g_object_set_data(G_OBJECT(button), "file-name", (gpointer)name);
    g_object_set_data(G_OBJECT(button), "dir", dir);
    g_signal_connect(button, "clicked",G_CALLBACK(on_file_clicked),user_data);
}
GtkWidget* file_select_window(GtkWidget *button,GFile *path){
    GtkDirectoryList *dirlist = gtk_directory_list_new("standard::*",path);
    GtkListItemFactory *factory = gtk_signal_list_item_factory_new();
    g_signal_connect(factory,"setup",G_CALLBACK(setup_cb),NULL);
    g_signal_connect(factory,"bind",G_CALLBACK(bind_cb),path);
    GtkWidget *list = gtk_grid_view_new(GTK_SELECTION_MODEL(gtk_single_selection_new(G_LIST_MODEL(dirlist))),GTK_LIST_ITEM_FACTORY(factory));
    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll),list);
    return scroll;
}