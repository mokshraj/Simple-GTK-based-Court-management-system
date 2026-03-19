#include <gtk/gtk.h>

/* -------- SETUP: create UI -------- */
void setup_cb(GtkSignalListItemFactory *factory,
              GtkListItem *list_item,
              gpointer user_data)
{
    GtkWidget *label = gtk_label_new(NULL);
    gtk_list_item_set_child(list_item, label);
}

/* -------- BIND: assign data -------- */
void bind_cb(GtkSignalListItemFactory *factory,
             GtkListItem *list_item,
             gpointer user_data)
{
    GtkWidget *label = gtk_list_item_get_child(list_item);

    const char *text = gtk_string_object_get_string(
        GTK_STRING_OBJECT(gtk_list_item_get_item(list_item)));

    gtk_label_set_text(GTK_LABEL(label), text);
}

/* -------- MAIN -------- */
static void activate(GtkApplication *app, gpointer data)
{
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Factory Example");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);

    /* 1. Create data model */
    const char *names[] = {"Alice", "Bob", "Charlie"};
    GListStore *store = g_list_store_new(GTK_TYPE_STRING_OBJECT);

    for (int i = 0; i < 3; i++) {
        g_list_store_append(store,
            gtk_string_object_new(names[i]));
    }

    /* 2. Create factory */
    GtkListItemFactory *factory =
    gtk_signal_list_item_factory_new();

    g_signal_connect(factory, "setup", G_CALLBACK(setup_cb), NULL);
    g_signal_connect(factory, "bind", G_CALLBACK(bind_cb), NULL);

    /* 3. Create list view */
    GtkWidget *list =
        gtk_list_view_new(
            GTK_SELECTION_MODEL(gtk_single_selection_new(G_LIST_MODEL(store))),
            GTK_LIST_ITEM_FACTORY(factory)
        );

    gtk_window_set_child(GTK_WINDOW(window), list);
    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv)
{
    GtkApplication *app =
        gtk_application_new("com.example.factory", G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    return g_application_run(G_APPLICATION(app), argc, argv);
}