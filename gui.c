#include "gui.h"
#include <string.h>
#include "JUDECIOMENT.h"
#include "get_executable_folder.h"
#include "clock.h"
#include <gio/gio.h>
#include "file_select.h"
#include <libsoup/soup.h>
#include <json-glib/json-glib.h>

static gboolean use_broadway = FALSE;
//
//
int main(int argc, char **argv)
{
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "web") == 0 || strcmp(argv[i], "--web") == 0) {
            use_broadway = TRUE;
            g_print("Using Broadway URL opener\n");
            break;
        }
    }
    
    pthread_attr_t dattr;
    pthread_attr_init(&dattr);
    pthread_attr_setdetachstate(&dattr, PTHREAD_CREATE_DETACHED);
    pthread_t a;
    int *return_int = g_new0(int, 1);
    pthread_create(&a, &dattr, (void *(*)(void *))creating_database, return_int);
    
    const char *app_id = g_getenv("GTK_APP_ID");
    if (!app_id) app_id = "org.gtk.example";
    
    GtkApplication *app;
    int status;
    app = gtk_application_new(app_id, G_APPLICATION_DEFAULT_FLAGS);
    
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    
    status = g_application_run(G_APPLICATION(app), 0, NULL);
    g_object_unref(app);
    pthread_attr_destroy(&dattr);
    
    return status;
}
//
//
static void switch_(GtkWidget *button, gpointer data){
    struct widget_and_id *a = (struct widget_and_id*)data;
    gtk_notebook_set_current_page(GTK_NOTEBOOK(a->widget),*(a->num));
}
//
void set(GtkWidget *widget,gpointer data){
    if (!gtk_check_button_get_active(GTK_CHECK_BUTTON(widget))) {
        // If it's not active (unchecked), do nothing
        return;
    }
    struct p_pair *a = (struct p_pair *)data;
    *(int*)(a->a) = *(int*)(a->b);
    printf("Value changed! %d \n",*(int*)(a->a));
}
void set_(GtkWidget *widget,gpointer data){
    widget_and_id *a = (widget_and_id *)data;
    *(a->num)  = *(int *)g_object_get_data(G_OBJECT(widget),"Schedule_id");
    widget_and_id *b = (widget_and_id *)g_object_get_data(G_OBJECT(a->widget),"Schedule_id_and_widget");
    gtk_label_set_label(GTK_LABEL(b->widget),gtk_button_get_label(GTK_BUTTON(widget)));
    b->num = a->num;
    gtk_widget_set_sensitive(a->widget, TRUE);
    printf("value changed %d %d\n",*(a->num),*(b->num));
    gpointer video_conn = g_object_get_data(G_OBJECT(a->widget),"conn");
    if(video_conn == NULL){
        GtkWidget *video_button = (GtkWidget *)g_object_get_data(G_OBJECT(a->widget), "Video_button");
        int type = *(int *)g_object_get_data(G_OBJECT(a->widget), "type");
        switch (type)
        {
            case 1:
                if (video_button && !g_signal_handler_find(video_button, G_SIGNAL_MATCH_FUNC, 0, 0, NULL, Video_hearing_start, NULL)) {
                    g_signal_connect(video_button, "clicked", G_CALLBACK(Video_hearing_start), b->num);
                    printf("Video_hearing_start connected!\n");
                }
                break;
            case 2:
                if (video_button && !g_signal_handler_find(video_button, G_SIGNAL_MATCH_FUNC, 0, 0, NULL, Video_hearing_open, NULL)) {
                    g_signal_connect(video_button, "clicked", G_CALLBACK(Video_hearing_open), b->num);
                    printf("Video_hearing_open connected!\n");
                }
                break;
            default:
                break;
        }
    }
}
//
/*Make widgets sensitive*/
void make_sensitive(GtkWidget *widget, gpointer data){
    g_print("\ni ran bro\n");
    GtkWidget  **widgets  = (GtkWidget **)data;
    int i = 0;
    while(widgets[i] != NULL){
        gtk_widget_set_sensitive(widgets[i], TRUE);
        i++;
    }
}
//
// void open_google_meet() {
// #if defined(_WIN32) || defined(_WIN64)
//     system("start https://meet.google.com/new");
// #elif defined(__APPLE__)
//     system("open https://meet.google.com/new");
// #else
//     system("xdg-open https://meet.google.com/new");
// #endif
// }
//
/* Callback for submit button - handles login verification */
void Login(GtkButton *widget, gpointer user_data){
    login_data *login = (login_data*)user_data;
    const char *pass = gtk_editable_get_text(GTK_EDITABLE(login->pass));
    const char* user = gtk_editable_get_text(GTK_EDITABLE(login->userid));
    printf("\n%s %s %d\n",pass,user,login->type);
    
    char *message;
    GtkWidget *parent = GTK_WIDGET(gtk_widget_get_root(GTK_WIDGET(widget)));
    GtkAlertDialog *alert = gtk_alert_dialog_new("Error!");
    gtk_alert_dialog_set_modal(alert, TRUE);
    const char *buttons[] = { "OK", NULL };
    gtk_alert_dialog_set_buttons(alert, buttons);
    gtk_alert_dialog_set_default_button(alert, 0);

    // Handle different login types
    switch (login->type) {
    case 0:
        gtk_alert_dialog_set_message(GTK_ALERT_DIALOG(alert),"Select login type");
        gtk_alert_dialog_show(GTK_ALERT_DIALOG(alert),GTK_WINDOW(parent));
        break;
    case 1:  // Judge login
        if(Judge_login(user,pass)){
            g_print("success");
            login->user_id = g_strdup(user);
            login->pass_ = g_strdup(pass);
            Judge_dash(parent,login);
        } else {
            gtk_alert_dialog_set_message(GTK_ALERT_DIALOG(alert),"Incorrect Id or password");
            gtk_alert_dialog_show(GTK_ALERT_DIALOG(alert),GTK_WINDOW(parent));
            g_print("fail");
        }
        break;
    case 2:  // Lawyer login
        if(Lawyer_login(user,pass)){
            g_print("success");
            login->user_id = g_strdup(user);
            login->pass_ = g_strdup(pass);
            Lawyer_dash(parent,login);
        } else {
            g_print("fail");
            gtk_alert_dialog_set_message(GTK_ALERT_DIALOG(alert),"Incorrect Id or password");
            gtk_alert_dialog_show(GTK_ALERT_DIALOG(alert),GTK_WINDOW(parent));
        }
        break;
    case 3:  // stenographer login
        if(strcmp(user,"Bro")==0 && strcmp(pass,"Bro")==0){
            g_print("success");
            login->user_id = g_strdup(user);
            login->pass_ = g_strdup(pass);
            Stenographer_dash(parent,login);
        } else {
            g_print("fail");
            gtk_alert_dialog_set_message(GTK_ALERT_DIALOG(alert),"Incorrect Id or password");
            gtk_alert_dialog_show(GTK_ALERT_DIALOG(alert),GTK_WINDOW(parent));
        }
        break;
    case 4:  // clerk login
        if(strcmp(user,"Bro")==0){
            g_print("success");
            login->user_id = g_strdup(user);
            login->pass_ = g_strdup(pass);
        } else {
            g_print("fail");
            gtk_alert_dialog_set_message(GTK_ALERT_DIALOG(alert),"Incorrect Id or password");
            gtk_alert_dialog_show(GTK_ALERT_DIALOG(alert),GTK_WINDOW(parent));
        }
        break;
    
    default:
        break;
    }

    g_object_unref(alert);
}
//
/* Helper function to get the primary monitor dimensions */
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
//
/* Window realize callback - sets initial window size */
void on_window_realize(GtkWidget *window, gpointer user_data) {
    GdkRectangle geometry = GetMonitorSize(window);
    gtk_window_set_resizable(GTK_WINDOW(window), TRUE);
    gtk_window_set_default_size(GTK_WINDOW(window),
                           (int)(geometry.width * 0.5),
                           (int)(geometry.height * 0.5)); // reduce initial height
}
/* Main application activation function */
void activate(GtkApplication *app, gpointer user_data)
{
    // Load CSS styling
    GtkCssProvider *provider = gtk_css_provider_new(); 
    gtk_css_provider_load_from_path(provider, "style.css");
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER
    );
    g_object_unref(provider);

    // Declare widgets
    GtkWidget *window, *button, *box, *grid, *entry, *pass, *check1, *check2, *check3, *check4;
    
    // Create and configure logo picture
    GtkWidget *picture = gtk_picture_new_for_filename("justice-law-svgrepo-com.svg");
    gtk_picture_set_can_shrink(GTK_PICTURE(picture), TRUE);
    gtk_picture_set_content_fit(GTK_PICTURE(picture), GTK_CONTENT_FIT_CONTAIN);
    gtk_widget_set_hexpand(picture, TRUE);
    gtk_widget_set_vexpand(picture, TRUE);
    gtk_widget_add_css_class(picture, "logo");
    
    // Create and configure main window
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Court-manager");
    g_signal_connect(window, "realize", G_CALLBACK(on_window_realize), NULL);
    gtk_window_present(GTK_WINDOW(window));
    
    // Get monitor geometry and set window size
    GdkRectangle geometry = GetMonitorSize(window);
    gtk_window_set_resizable(GTK_WINDOW(window), TRUE);
    gtk_window_set_default_size(GTK_WINDOW(window),
                           (int)(geometry.width * 0.5),
                           (int)(geometry.height * 0.45));
    
    // Create grid for form layout
    grid = gtk_grid_new();
    
    // Create vertical box and add picture and grid
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_box_append(GTK_BOX(box), picture);
    gtk_box_append(GTK_BOX(box), grid);
    gtk_box_set_homogeneous(GTK_BOX(box), TRUE);
    
    // Set alignments for grid and box
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
    gtk_window_set_child(GTK_WINDOW(window), box);
    
    // Create and configure user ID entry
    entry = gtk_entry_new();
    gtk_widget_set_size_request(entry, (geometry.width)*0.2, -1); 
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Enter user ID");
    gtk_grid_attach(GTK_GRID(grid), entry, 0, 0, 2, 1);
    
    // Create and configure password entry
    pass = gtk_password_entry_new();
    g_object_set(pass, "placeholder-text", "Enter your password", NULL);
    gtk_password_entry_set_show_peek_icon(GTK_PASSWORD_ENTRY(pass), TRUE);
    gtk_grid_attach(GTK_GRID(grid), pass, 0, 1, 2, 1);
    
    // Create submit button
    button = gtk_button_new_with_label("submit");
    gtk_widget_add_css_class(button, "special");
    
    // Initialize login data structure
    login_data *login = g_new(login_data, 1);
    login->userid = entry;
    login->pass = pass;
    login->type = 0;
    int *one = g_new(int,4);
    one[0] = 1;one[1] = 2;one[2] = 3;one[3] = 4;
    
    // Connect submit button click handler
    g_signal_connect(button, "clicked", G_CALLBACK(Login), login);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 2, 1, 1);
    
    // Create and connect close button
    button = gtk_button_new_with_label("close");
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), window);
    gtk_grid_attach(GTK_GRID(grid), button, 1, 2, 1, 1);
    
    // Create Judge checkbox
    check1 = gtk_check_button_new_with_label("Judge");
    g_signal_connect(check1, "toggled",G_CALLBACK(set),g_memdup2(&(struct p_pair){(void **)&login->type,&one[0]}, sizeof(struct p_pair)));
    gtk_grid_attach(GTK_GRID(grid), check1, 2, 0, 1, 1);

    check2 = gtk_check_button_new_with_label("Lawyer");
    gtk_check_button_set_group(GTK_CHECK_BUTTON(check2), GTK_CHECK_BUTTON(check1));
    g_signal_connect(check2, "toggled",G_CALLBACK(set),g_memdup2(&(struct p_pair){(void **)&login->type,&one[1]}, sizeof(struct p_pair)));
    gtk_grid_attach(GTK_GRID(grid), check2, 2, 1, 1, 1);

    check3 = gtk_check_button_new_with_label("Stenographer");
    gtk_check_button_set_group(GTK_CHECK_BUTTON(check3), GTK_CHECK_BUTTON(check1));
    g_signal_connect(check3, "toggled",G_CALLBACK(set),g_memdup2(&(struct p_pair){(void **)&login->type,&one[2]}, sizeof(struct p_pair)));
    gtk_grid_attach(GTK_GRID(grid), check3, 2, 2, 1, 1);

    check4 = gtk_button_new_with_label("Cleark");
    g_signal_connect(check4, "clicked",G_CALLBACK(open_file),g_strdup("JUDECIOMENT"));
    gtk_grid_attach(GTK_GRID(grid), check4, 0, 3, 2, 1);
    check4 = gtk_button_new_with_label("Schedules");
    g_signal_connect(check4, "clicked",G_CALLBACK(gui_open_file),g_strdup("Schedule"));
    gtk_grid_attach(GTK_GRID(grid), check4, 0, 4, 2, 1);
    // check4 = gtk_button_new_with_label("AI Bot");
    // g_signal_connect(check4, "clicked",G_CALLBACK(open_python_file),g_strdup("newcahtbot.py"));
    // gtk_grid_attach(GTK_GRID(grid), check4, 0, 5, 2, 1);
    // Free login data when window is destroyed
    g_signal_connect_swapped(window, "destroy", G_CALLBACK(g_free), login);
}
//
//
static void Schedule_grid_contraints(GtkLayoutManager *layout,GtkWidget*container,GtkWidget*box,GdkRectangle geometry){
    gtk_constraint_layout_add_constraint(GTK_CONSTRAINT_LAYOUT(layout),
    gtk_constraint_new(
        GTK_CONSTRAINT_TARGET(box),
        GTK_CONSTRAINT_ATTRIBUTE_START,
        GTK_CONSTRAINT_RELATION_EQ,
        GTK_CONSTRAINT_TARGET(container),
        GTK_CONSTRAINT_ATTRIBUTE_START,
        1.0,
        geometry.width *0.015,
        GTK_CONSTRAINT_STRENGTH_REQUIRED
    )
    );
    gtk_constraint_layout_add_constraint(GTK_CONSTRAINT_LAYOUT(layout),
        gtk_constraint_new(
            GTK_CONSTRAINT_TARGET(box),
            GTK_CONSTRAINT_ATTRIBUTE_TOP,
            GTK_CONSTRAINT_RELATION_EQ,
            GTK_CONSTRAINT_TARGET(container),
            GTK_CONSTRAINT_ATTRIBUTE_TOP,
            1.0,
            geometry.height *0.017,
            GTK_CONSTRAINT_STRENGTH_REQUIRED
        )
    );
    gtk_constraint_layout_add_constraint(GTK_CONSTRAINT_LAYOUT(layout),
        gtk_constraint_new(
            GTK_CONSTRAINT_TARGET(box),
            GTK_CONSTRAINT_ATTRIBUTE_BOTTOM,
            GTK_CONSTRAINT_RELATION_EQ,
            GTK_CONSTRAINT_TARGET(container),
            GTK_CONSTRAINT_ATTRIBUTE_BOTTOM,
            1.0,
            -geometry.height *0.017,
            GTK_CONSTRAINT_STRENGTH_REQUIRED
        )
    );
    // 30% WIDTH (only ONCE)
    gtk_constraint_layout_add_constraint(GTK_CONSTRAINT_LAYOUT(layout),
        gtk_constraint_new(
            GTK_CONSTRAINT_TARGET(box),
            GTK_CONSTRAINT_ATTRIBUTE_WIDTH,
            GTK_CONSTRAINT_RELATION_EQ,
            GTK_CONSTRAINT_TARGET(container),
            GTK_CONSTRAINT_ATTRIBUTE_WIDTH,
            0.30,
            0,
            GTK_CONSTRAINT_STRENGTH_REQUIRED
        )
    );
    gtk_constraint_layout_add_constraint(GTK_CONSTRAINT_LAYOUT(layout),
        gtk_constraint_new(
            GTK_CONSTRAINT_TARGET(box),
            GTK_CONSTRAINT_ATTRIBUTE_HEIGHT,
            GTK_CONSTRAINT_RELATION_EQ,
            GTK_CONSTRAINT_TARGET(container),
            GTK_CONSTRAINT_ATTRIBUTE_HEIGHT,
            1.0,
            0,
            GTK_CONSTRAINT_STRENGTH_REQUIRED
        )
    );
}
//
/*functions to open files and dialogs*/
static void dialog_open_file(GObject *source_object, GAsyncResult *res, gpointer user_data)
{
    GtkFileDialog *dialog = GTK_FILE_DIALOG(source_object);
    GFile *file = gtk_file_dialog_open_finish(dialog, res, NULL);

    if (file) {
        char *path = g_file_get_path(file);
        g_print("Selected file: %s\n", path);
        #if defined(_WIN32) || defined(_WIN64)
            char *command = g_strdup_printf("start %s",path);
        #elif defined(__APPLE__)
            char *command = g_strdup_printf("open %s",path);
        #else
            char *command = g_strdup_printf("xdg-open %s",path);
        #endif
        system(command);
        g_free(command);
        g_free(path);
        g_object_unref(file);
    } else {
        g_print("No file selected or dialog cancelled.\n");
    }
}
static void open_file(GtkWidget *button,gpointer relative)
{
    
    char *path = g_strdup_printf("%s/%s",get_executable_folder(),(char *)relative);
    g_print("Selected file: %s\n", path);
    #if defined(_WIN32) || defined(_WIN64)
        char *command = g_strdup_printf("%s",path);
    #elif defined(__APPLE__)
        char *command = g_strdup_printf("%s",path);
    #else
        char *command = g_strdup_printf("kgx -e %s",path);
    #endif
    system(command);
    g_free(command);
    g_free(path);
}
static void gui_open_file(GtkWidget *button,gpointer relative)
{
    
    char *path = g_strdup_printf("%s/%s",get_executable_folder(),(char *)relative);
    g_print("Selected file: %s\n", path);
    #if defined(_WIN32) || defined(_WIN64)
        char *command = g_strdup_printf("%s",path);
    #elif defined(__APPLE__)
        char *command = g_strdup_printf("%s",path);
    #else
        char *command = g_strdup_printf("%s",path);
    #endif
    system(command);
    g_free(command);
    g_free(path);
}
static void open_python_file(GtkWidget *button,gpointer relative)
{
    
    char *path = g_strdup_printf("%s/%s",get_executable_folder(),(char *)relative);
    g_print("Selected file: %s\n", path);
    #if defined(_WIN32) || defined(_WIN64)
        char *command = g_strdup_printf("python %s",path);
    #elif defined(__APPLE__)
        char *command = g_strdup_printf("python %s",path);
    #else
        char *command = g_strdup_printf("python %s",path);
    #endif
    system(command);
    g_free(command);
    g_free(path);
}
//
static void open_folder(GtkWidget *button, gpointer user_data)
{
    widget_and_id *pair = (widget_and_id *)g_object_get_data(G_OBJECT(user_data),"Schedule_id_and_widget");
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
    sql = g_strdup_printf("SELECT Case_id FROM case_schedule WHERE schedule_id = %d ;",*(pair->num));
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_step(stmt);
    int case_id = sqlite3_column_int(stmt, 0);
    g_print("%d",case_id);
    g_free(sql);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    GtkWindow *parent = GTK_WINDOW(gtk_widget_get_root(button));
    // GtkFileDialog *dialog = gtk_file_dialog_new();
    char * path = g_strdup_printf("%s/Cases/%d/%s/",get_executable_folder(),case_id,(char *)g_object_get_data(G_OBJECT(button),"Folder"));
    g_print(path);
    GFile *folder = g_file_new_for_path(path);
    ensure_directory_exists(path);
    GtkWidget *file_explorer = file_select_window(GTK_WIDGET(parent),folder);
    // gtk_file_dialog_set_initial_folder(dialog, folder);
    g_object_unref(folder);
    // GtkFileFilter *filter = gtk_file_filter_new();

    // gtk_file_dialog_open(dialog, parent, NULL, dialog_open_file, NULL);
    gtk_window_present(GTK_WINDOW(file_explorer));
    GdkRectangle size = GetMonitorSize(file_explorer);
    gtk_window_set_default_size(GTK_WINDOW(file_explorer),size.width * 0.5 ,size.height *0.5);
}
//
//
GtkWidget *CaseControlPanel(){
    GtkWidget *grid = gtk_grid_new();
    GtkWidget *label = gtk_label_new("Case ID : , ");
    gtk_widget_set_hexpand(label,TRUE);
    GtkWidget *button = gtk_button_new_with_label("View Case Files");
    gtk_widget_set_hexpand(button,TRUE);
    gtk_widget_set_vexpand(button,TRUE);
    g_object_set_data_full(G_OBJECT(grid),"Schedule_id_and_widget",g_memdup2(&(widget_and_id){label,NULL},sizeof(widget_and_id)),g_free);
    g_object_set_data_full(G_OBJECT(button),"Folder",(gpointer)g_strdup("Files"),g_free);
    gtk_grid_attach(GTK_GRID(grid),label,0,0,2,1);
    gtk_grid_attach(GTK_GRID(grid),button,0,1,2,1);
    g_signal_connect(button,"clicked",G_CALLBACK(open_folder),grid);
    button = gtk_button_new_with_label("Open Evidenes");
    gtk_widget_set_hexpand(button,TRUE);
    gtk_widget_set_vexpand(button,TRUE);
    g_object_set_data_full(G_OBJECT(button),"Folder",(gpointer)g_strdup("Evidences"),g_free);
    g_signal_connect(button,"clicked",G_CALLBACK(open_folder),grid);
    gtk_grid_attach(GTK_GRID(grid),button,0,2,2,1);
    GtkWidget *video_button = gtk_button_new_with_label("Video hearing");
    gtk_widget_set_hexpand(video_button,TRUE);
    gtk_widget_set_vexpand(video_button,TRUE);
    g_object_set_data(G_OBJECT(grid), "Video_button", video_button);
    g_object_set_data(G_OBJECT(grid),"type",g_memdup2(&(int){1},sizeof(int)));
    gtk_grid_attach(GTK_GRID(grid),video_button,0,3,2,1);
    gtk_widget_set_sensitive(grid, FALSE);
    return grid;
}

static void CaseControlPanel_contraints(GtkLayoutManager *layout,GtkWidget*box,GtkWidget*CaseControlPanel,GdkRectangle geometry){
    gtk_constraint_layout_add_constraint(GTK_CONSTRAINT_LAYOUT(layout),
    gtk_constraint_new(
        GTK_CONSTRAINT_TARGET(CaseControlPanel),
        GTK_CONSTRAINT_ATTRIBUTE_START,
        GTK_CONSTRAINT_RELATION_EQ,
        GTK_CONSTRAINT_TARGET(box),
        GTK_CONSTRAINT_ATTRIBUTE_END,
        1.0,
        geometry.width *0.015,
        GTK_CONSTRAINT_STRENGTH_REQUIRED
    )
    );
    gtk_constraint_layout_add_constraint(GTK_CONSTRAINT_LAYOUT(layout),
        gtk_constraint_new(
            GTK_CONSTRAINT_TARGET(CaseControlPanel),
            GTK_CONSTRAINT_ATTRIBUTE_TOP,
            GTK_CONSTRAINT_RELATION_EQ,
            GTK_CONSTRAINT_TARGET(box),
            GTK_CONSTRAINT_ATTRIBUTE_TOP,
            1.0,
            0,
            GTK_CONSTRAINT_STRENGTH_REQUIRED
        )
    );
    gtk_constraint_layout_add_constraint(GTK_CONSTRAINT_LAYOUT(layout),
        gtk_constraint_new(
            GTK_CONSTRAINT_TARGET(CaseControlPanel),
            GTK_CONSTRAINT_ATTRIBUTE_BOTTOM,
            GTK_CONSTRAINT_RELATION_EQ,
            GTK_CONSTRAINT_TARGET(box),
            GTK_CONSTRAINT_ATTRIBUTE_BOTTOM,
            1.0 * 0.30,
            0,
            GTK_CONSTRAINT_STRENGTH_REQUIRED
        )
    );
    // 30% WIDTH (only ONCE)
    gtk_constraint_layout_add_constraint(GTK_CONSTRAINT_LAYOUT(layout),
        gtk_constraint_new(
            GTK_CONSTRAINT_TARGET(CaseControlPanel),
            GTK_CONSTRAINT_ATTRIBUTE_WIDTH,
            GTK_CONSTRAINT_RELATION_EQ,
            GTK_CONSTRAINT_TARGET(gtk_widget_get_parent(box)),
            GTK_CONSTRAINT_ATTRIBUTE_WIDTH,
            0.30,
            0,
            GTK_CONSTRAINT_STRENGTH_REQUIRED
        )
    );
    
}
//
//
//
/*Schedule list for panel*/
GtkWidget *Schedule_list(login_data *login,int *set_id,GtkWidget *casecontrol,GDateTime* Date_time,GtkWidget **widgets){
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
    char *useridint = g_strdup((login->user_id) + 2);
    printf(" %s",useridint);
    if (login->type == 1) {
        sql = g_strdup_printf(
            "SELECT cs.case_id, ct.Name, cs.schedule_id "
            "FROM case_schedule cs "
            "INNER JOIN Crime_table ct ON cs.case_id = ct.Case_ID "
            "WHERE ct.Judge_ID = %s AND cs.date = %s;",
            useridint, Date
        );
    }
    else if (login->type == 2) {
        sql = g_strdup_printf(
            "SELECT cs.case_id, ct.Name, cs.schedule_id "
            "FROM case_schedule cs "
            "INNER JOIN Crime_table ct ON cs.case_id = ct.Case_ID "
            "WHERE (ct.Lyr1_ID = %s OR ct.Lyr2_ID = %s) AND cs.date = %s;",
            useridint, useridint, Date
        );
    }
    else if (login->type == 3) {
        sql = g_strdup_printf(
            "SELECT cs.case_id, ct.Name, cs.schedule_id "
            "FROM case_schedule cs "
            "INNER JOIN Crime_table ct ON cs.case_id = ct.Case_ID "
            "WHERE cs.date = %s;",
            Date
        );
    }
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(casecontrol != NULL){
        if(widgets == NULL){
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                int case_id = sqlite3_column_int(stmt, 0);
                const unsigned char *name = sqlite3_column_text(stmt, 1);
                int schedule_id = sqlite3_column_int(stmt,2);
                button =  gtk_button_new_with_label(g_strdup_printf("Case ID : %d, %s",case_id,name));
                g_object_set_data_full(G_OBJECT(button),"Schedule_id",g_memdup2(&(int){schedule_id},sizeof(int)),g_free);
                g_signal_connect(button,"clicked",G_CALLBACK(set_),g_memdup2(&(widget_and_id){casecontrol,set_id},sizeof(widget_and_id)));
                gtk_box_append(GTK_BOX(list_box),button);
            }
        }
        else{
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                int case_id = sqlite3_column_int(stmt, 0);
                const unsigned char *name = sqlite3_column_text(stmt, 1);
                int schedule_id = sqlite3_column_int(stmt,2);
                button =  gtk_button_new_with_label(g_strdup_printf("Case ID : %d, %s",case_id,name));
                g_object_set_data_full(G_OBJECT(button),"Schedule_id",g_memdup2(&(int){schedule_id},sizeof(int)),g_free);
                g_signal_connect(button,"clicked",G_CALLBACK(set_),g_memdup2(&(widget_and_id){casecontrol,set_id},sizeof(widget_and_id)));
                g_signal_connect(button,"clicked",G_CALLBACK(make_sensitive),widgets);
                gtk_box_append(GTK_BOX(list_box),button);
            }
        }
    }
    g_free(Date);
    g_free(sql);
    g_free(useridint);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    // Set the box as the scrolled window's child
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(Schedule_list), list_box);
    gtk_scrolled_window_set_has_frame(GTK_SCROLLED_WINDOW(Schedule_list),TRUE);
    return Schedule_list;
}
//
/*Judge's dashboard*/
void Judge_dash(GtkWidget *window, gpointer user_data){
    //Login data
    login_data *login = (login_data*)user_data;
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
    gtk_window_set_child(GTK_WINDOW(window), NULL);
    GdkRectangle geometry = GetMonitorSize(window);
    gtk_window_set_resizable(GTK_WINDOW(window), TRUE);
    gtk_window_maximize(GTK_WINDOW(window));
    //
    GtkWidget *container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,2);
    GtkLayoutManager *layout = gtk_constraint_layout_new();
    gtk_widget_set_layout_manager(container,layout);
    gtk_window_set_child(GTK_WINDOW(window), container);
    //
    // Schedule
    //
    int *schedule_id = g_new0(int, 1);
    GtkWidget *box = gtk_grid_new();
    GtkWidget *icon1 = gtk_image_new_from_file("schedule-time-and-date-svgrepo-com.svg");
    GtkWidget *label1 = gtk_label_new("schedule");
    GtkWidget *CaseControlPanel_ = CaseControlPanel(); 
    GtkWidget *HearingTime = clock_(schedule_id);
    gtk_widget_add_css_class(label1,"label");
    gtk_widget_add_css_class(icon1,"label");
    gtk_image_set_pixel_size(GTK_IMAGE(icon1),geometry.width * 0.02);
    gtk_grid_attach(GTK_GRID(box),icon1,0,0,1,1);
    gtk_grid_attach(GTK_GRID(box),label1,1,0,1,1);
    GtkWidget **widgets = g_new(GtkWidget *,4);
    widgets[0] = HearingTime;
    gtk_grid_attach(GTK_GRID(box),Schedule_list(login,schedule_id,CaseControlPanel_,NULL,widgets),0,1,2,1);
    GtkWidget *button = gtk_button_new_with_label("Open Future Schedules");
    GdkRectangle *copy = g_new(GdkRectangle, 1); // allocates memory
    *copy = geometry;                            // copy struct contents
    g_object_set_data_full(G_OBJECT(button), "geometry", copy, g_free);
    g_object_set_data(G_OBJECT(button),"login",login);
    g_object_set_data(G_OBJECT(button),"schedule_id",schedule_id);
    g_object_set_data(G_OBJECT(button),"Control_panel",CaseControlPanel_);
    g_signal_connect(button,"clicked",G_CALLBACK(Future_schedule_panel),widgets);
    gtk_grid_attach(GTK_GRID(box),button,0,2,2,1);
    gtk_box_append(GTK_BOX(container),box);
    //
    char *file = g_strdup("Judge_notes.txt");
    GtkWidget *notes = Textbox(schedule_id,file,login,"Judge_notes");
    gtk_widget_add_css_class(notes,"scroll");
    gtk_box_append(GTK_BOX(container),notes);
    note_box_contraints(layout,CaseControlPanel_,box,notes,geometry);
    //
    char *file2 = g_strdup("Info.txt");
    GtkWidget *Info = Infobox(schedule_id,file2,login);
    gtk_widget_add_css_class(Info,"scroll");
    gtk_box_append(GTK_BOX(container),Info);
    info_box_contraints(layout,CaseControlPanel_,notes,Info,geometry);
    //
    widgets[1] = notes;
    widgets[2] = Info;
    widgets[3] = NULL;
    gtk_grid_attach(GTK_GRID(box),HearingTime,0,3,2,1);
    Schedule_grid_contraints(layout,container,box,geometry);
    gtk_box_append(GTK_BOX(container),CaseControlPanel_);
    CaseControlPanel_contraints(layout,box,CaseControlPanel_,geometry);

}
static void update_video_id(GtkWidget *widget,gpointer data){
    char *err_msg = NULL;
    char *link = g_strdup(gtk_editable_get_text(GTK_EDITABLE((GtkWidget *)data)));
    char * sql = g_strdup_printf("UPDATE case_schedule SET Video_id = \"%s\" WHERE schedule_id = %d;",link,*(int *)g_object_get_data(G_OBJECT(widget),"Schedule_id"));
    sqlite3 *db;
    int rc = sqlite3_open("Judgment.db", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return;
    }
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        printf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
    fflush(stdout);
    g_free(link);
    g_free(sql);
    sqlite3_close(db);
}
static int case_callback(void *data, int argc, char **argv, char **azColName) {
    if (argc > 0 && argv[0]) {
        int *case_id = (int *)data;
        *case_id = atoi(argv[0]);  // string → int
    }
    return 0;
}
// static void Video_hearing_start(GtkWidget *widget,gpointer data){
//     int case_id = -1;
//     char *err_msg = NULL;

//     sqlite3 *db;
//     int rc = sqlite3_open("Judgment.db", &db);

//     if (rc != SQLITE_OK) {
//         fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
//         return;
//     }

//     // 🔹 schedule_id (assuming data is int*)
//     int schedule_id = *(int *)data;

//     char *sql = g_strdup_printf(
//         "SELECT case_id FROM case_schedule WHERE schedule_id = %d;",
//         schedule_id
//     );

//     // 🔹 IMPORTANT: pass callback
//     rc = sqlite3_exec(db, sql, case_callback, &case_id, &err_msg);

//     if (rc != SQLITE_OK) {
//         printf("SQL error: %s\n", err_msg);
//         sqlite3_free(err_msg);
//     } else {
//         printf("Case ID: %d\n", case_id);
//     }

//     g_free(sql);
//     sqlite3_close(db);
//     char * path = g_strdup_printf("%s/Cases/%d/%s/",get_executable_folder(),case_id,"recordings");
//     g_free(path);
    
//     system(path);
//     g_free(path);
//     GtkWidget *Parent = GTK_WIDGET(gtk_widget_get_root(GTK_WIDGET(widget)));
//     GtkWidget *Dialog = gtk_window_new();
//     GtkWidget *Entry = gtk_entry_new();
//     GtkWidget *Button = gtk_button_new_with_label("Ok");
//     GtkWidget *Box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
//     g_object_set_data(G_OBJECT(Button),"Schedule_id",data);
//     g_signal_connect(GTK_BUTTON(Button),"clicked",G_CALLBACK(update_video_id),Entry);
//     gtk_entry_set_placeholder_text(GTK_ENTRY(Entry),"Enter meet code ");
//     gtk_window_set_title(GTK_WINDOW(Dialog),"Enter Meeting Link");
//     gtk_window_set_transient_for(GTK_WINDOW(Dialog),GTK_WINDOW(Parent));
//     gtk_window_set_modal(GTK_WINDOW(Dialog),TRUE);
//     gtk_box_append(GTK_BOX(Box),Entry);
//     gtk_box_append(GTK_BOX(Box),Button);
//     gtk_window_set_child(GTK_WINDOW(Dialog),Box);
//     open_google_meet();
//     gtk_window_present(GTK_WINDOW(Dialog));
// }

/* ---------- WebSocket helpers (adapted from test2.c) ---------- */

typedef struct {
    int schedule_id;
    int case_id;
    char *recording_path;
    SoupSession *session;
    GtkWindow *parent;
} MeetingStartData;

static void ws_on_closed(SoupWebsocketConnection *conn, gpointer user_data)
{
    MeetingStartData *msd = user_data;
    g_print("WebSocket closed\n");

    if (msd->session)
        g_object_unref(msd->session);
    g_free(msd->recording_path);
    g_free(msd);
    g_object_unref(conn);
}
//* Broadway-compatible URL opener: notifies Go portal instead of system() */
static void broadway_open_url(const char *url)
{
    const char *session_id = g_getenv("SESSION_ID");
    if (!session_id || !url || *url == '\0') {
        g_printerr("Cannot open URL: no SESSION_ID or empty URL\n");
        return;
    }

    char *encoded = g_uri_escape_string(url, NULL, FALSE);
    char *notify_url = g_strdup_printf(
        "https://localhost:1753/notify?id=%s&url=%s",
        session_id, encoded);

    SoupSession *sess = soup_session_new();
    SoupMessage *msg = soup_message_new("GET", notify_url);
    
    GError *error = NULL;
    GBytes *response = soup_session_send_and_read(sess, msg, NULL, &error);
    
    if (error != NULL) {
        g_printerr("Failed to notify portal: %s\n", error->message);
        g_error_free(error);
    } else {
        guint status = soup_message_get_status(msg);
        if (status != 200) {
            g_printerr("Failed to notify portal: HTTP %u\n", status);
        } else {
            g_print("Notified portal to open: %s\n", url);
        }
    }

    if (response != NULL)
        g_bytes_unref(response);
    
    g_free(encoded);
    g_free(notify_url);
    g_object_unref(msg);
    g_object_unref(sess);
}

static void ws_on_message(SoupWebsocketConnection *conn,
                          SoupWebsocketDataType type,
                          GBytes *message,
                          gpointer user_data)
{
    MeetingStartData *msd = user_data;

    if (type != SOUP_WEBSOCKET_DATA_TEXT)
        return;

    gsize size;
    const char *text = g_bytes_get_data(message, &size);
    g_print("\n📩 Server: %.*s\n", (int)size, text);

    JsonParser *parser = json_parser_new();
    GError *error = NULL;

    if (!json_parser_load_from_data(parser, text, size, &error)) {
        g_printerr("JSON parse error: %s\n", error->message);
        g_error_free(error);
        g_object_unref(parser);
        soup_websocket_connection_close(conn, SOUP_WEBSOCKET_CLOSE_NORMAL, NULL);
        return;
    }

    JsonNode *root = json_parser_get_root(parser);
    if (!JSON_NODE_HOLDS_OBJECT(root)) {
        g_object_unref(parser);
        soup_websocket_connection_close(conn, SOUP_WEBSOCKET_CLOSE_NORMAL, NULL);
        return;
    }

    JsonObject *obj = json_node_get_object(root);
    const char *msg_type = NULL;

    if (json_object_has_member(obj, "type"))
        msg_type = json_object_get_string_member(obj, "type");

    /* Handle the server's meeting_link response */
    if (msg_type && strcmp(msg_type, "meeting_link") == 0 &&
        json_object_has_member(obj, "payload")) {
        
        JsonObject *payload = json_object_get_object_member(obj, "payload");
        const char *url = NULL;

        if (json_object_has_member(payload, "url"))
            url = json_object_get_string_member(payload, "url");

        if (url) {
            g_print("🔗 Meeting URL: %s\n", url);

            /* Save URL to database as Video_id */
            sqlite3 *db;
            int rc = sqlite3_open("Judgment.db", &db);
            if (rc == SQLITE_OK) {
                char *sql = g_strdup_printf(
                    "UPDATE case_schedule SET Video_id = \"%s\" WHERE schedule_id = %d;",
                    url, msd->schedule_id);
                char *err_msg = NULL;
                sqlite3_exec(db, sql, 0, 0, &err_msg);
                if (err_msg) {
                    g_printerr("SQL error: %s\n", err_msg);
                    sqlite3_free(err_msg);
                }
                g_free(sql);
                sqlite3_close(db);
            }

//             /* Open browser (cross-platform) */
// #if defined(_WIN32) || defined(_WIN64)
//             char *cmd = g_strdup_printf("start %s", url);
// #elif defined(__APPLE__)
//             char *cmd = g_strdup_printf("open %s", url);
// #else
//             char *cmd = g_strdup_printf("xdg-open %s", url);
// #endif
//             int ret = system(cmd);
//             if (ret != 0)
//                 g_printerr("Failed to open browser (exit code %d)\n", ret);
//             g_free(cmd);
                        /* Open in client's browser via Go portal (Broadway-compatible) */
            if (use_broadway) {
                broadway_open_url(url);
            } else {
                GError *error = NULL;
                g_app_info_launch_default_for_uri(url, NULL, &error);
                if (error) {
                    g_printerr("Failed to open URL: %s\n", error->message);
                    g_error_free(error);
                }
            }
        }
    }

    g_object_unref(parser);
    soup_websocket_connection_close(conn, SOUP_WEBSOCKET_CLOSE_NORMAL, NULL);
}

static void ws_on_connect(GObject *source_object,
                          GAsyncResult *res,
                          gpointer user_data)
{
    MeetingStartData *msd = user_data;
    GError *error = NULL;

    SoupWebsocketConnection *conn = soup_session_websocket_connect_finish(
        SOUP_SESSION(source_object), res, &error);

    if (error) {
        g_printerr("❌ WebSocket connection failed: %s\n", error->message);
        g_error_free(error);
        if (msd->session) g_object_unref(msd->session);
        g_free(msd->recording_path);
        g_free(msd);
        return;
    }

    g_print("Connected to meeting server\n");

    g_signal_connect(conn, "message", G_CALLBACK(ws_on_message), msd);
    g_signal_connect(conn, "closed",  G_CALLBACK(ws_on_closed), msd);

    /* Build JSON: {"type": "open_meeting"} */
    JsonBuilder *builder = json_builder_new();
    json_builder_begin_object(builder);
    json_builder_set_member_name(builder, "type");
    json_builder_add_string_value(builder, "open_meeting");
    json_builder_end_object(builder);

    JsonGenerator *gen = json_generator_new();
    json_generator_set_root(gen, json_builder_get_root(builder));

    gsize len;
    char *json_str = json_generator_to_data(gen, &len);

    soup_websocket_connection_send_text(conn, json_str);
    g_print("Sent: %s\n", json_str);

    g_free(json_str);
    g_object_unref(gen);
    g_object_unref(builder);
}

typedef struct {
    char *url;
} OpenLinkData;

static void open_link_ws_closed(SoupWebsocketConnection *conn, gpointer user_data)
{
    OpenLinkData *data = user_data;
    g_free(data->url);
    g_free(data);
    g_object_unref(conn);
}

static void open_link_ws_connect(GObject *source, GAsyncResult *res, gpointer user_data)
{
    OpenLinkData *data = user_data;
    GError *error = NULL;
    SoupWebsocketConnection *conn = soup_session_websocket_connect_finish(
        SOUP_SESSION(source), res, &error);

    if (error) {
        g_printerr("Link relay connection failed: %s\n", error->message);
        g_error_free(error);
        g_free(data->url);
        g_free(data);
        return;
    }

    /* Build JSON: {"type":"open_link","url":"..."} */
    JsonBuilder *builder = json_builder_new();
    json_builder_begin_object(builder);
    json_builder_set_member_name(builder, "type");
    json_builder_add_string_value(builder, "open_link");
    json_builder_set_member_name(builder, "url");
    json_builder_add_string_value(builder, data->url);
    json_builder_end_object(builder);

    JsonGenerator *gen = json_generator_new();
    json_generator_set_root(gen, json_builder_get_root(builder));
    gsize len;
    char *json_str = json_generator_to_data(gen, &len);

    soup_websocket_connection_send_text(conn, json_str);
    g_print("Sent link to backend: %s\n", json_str);

    g_free(json_str);
    g_object_unref(gen);
    g_object_unref(builder);

    g_signal_connect(conn, "closed", G_CALLBACK(open_link_ws_closed), data);
    soup_websocket_connection_close(conn, SOUP_WEBSOCKET_CLOSE_NORMAL, NULL);
}

/* 
 * Call this from any GTK callback to ask the Go backend to open `url`
 * in the connected browser.
 */
void send_link_to_browser(const char *url)
{
    g_return_if_fail(url != NULL);

    SoupSession *session = soup_session_new();
    soup_session_set_tls_interaction(session, NULL);
    GTlsInteraction *interaction = NULL;
    GError *tls_err = NULL;
    char *cert_path = g_strdup_printf("%s/cert.pem", get_executable_folder());
    GTlsDatabase *tls_db = g_tls_file_database_new(cert_path, &tls_err);
    g_free(cert_path);
    if (tls_db) {
        soup_session_set_tls_database(session, tls_db);
        g_object_unref(tls_db);
    }
    SoupMessage *msg = soup_message_new("GET", "wss://localhost:1755/relay?role=gui");

    OpenLinkData *data = g_new0(OpenLinkData, 1);
    data->url = g_strdup(url);

    soup_session_websocket_connect_async(
        session, msg, NULL, NULL,
        G_PRIORITY_DEFAULT, NULL,
        open_link_ws_connect, data);

    g_object_unref(msg);
    g_object_unref(session);
}

/* ---------- Modified Video_hearing_start ---------- */


static void Video_hearing_start(GtkWidget *widget, gpointer data)
{
    int schedule_id = *(int *)data;
    printf("%d", schedule_id);
    fflush(stdout);

    /* Query case_id from database */
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc = sqlite3_open("Judgment.db", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    char *sql = g_strdup_printf(
        "SELECT case_id FROM case_schedule WHERE schedule_id = %d;", schedule_id);

    int case_id = -1;
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc == SQLITE_OK && sqlite3_step(stmt) == SQLITE_ROW) {
        case_id = sqlite3_column_int(stmt, 0);
    }

    g_free(sql);
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    if (case_id == -1) {
        g_printerr("Could not find case for schedule %d\n", schedule_id);
        return;
    }

    printf("Case ID: %d\n", case_id);

    /* Ensure recordings directory exists */
    char *path = g_strdup_printf("%s/Cases/%d/recordings/", get_executable_folder(), case_id);
    ensure_directory_exists(path);

    /* Prepare async data and connect to WebSocket server */
    MeetingStartData *msd = g_new0(MeetingStartData, 1);
    msd->schedule_id = schedule_id;
    msd->case_id = case_id;
    msd->recording_path = path;
    msd->parent = GTK_WINDOW(gtk_widget_get_root(widget));

    msd->session = soup_session_new();
    char *cert_path = g_strdup_printf("%s/cert.pem", get_executable_folder());
    GError *tls_err = NULL;
    GTlsDatabase *tls_db = g_tls_file_database_new(cert_path, &tls_err);
    g_free(cert_path);
    if (tls_db) {
        soup_session_set_tls_database(msd->session, tls_db);
        g_object_unref(tls_db);
    }
    SoupMessage *msg = soup_message_new("GET", "wss://localhost:1755/ws?role=client");

    soup_session_websocket_connect_async(
        msd->session,
        msg,
        NULL, NULL,
        G_PRIORITY_DEFAULT,
        NULL,
        ws_on_connect,
        msd
    );

    g_object_unref(msg);
}
// static void Video_hearing_open(GtkWidget *widget,gpointer data){
//     int schedule_id = *(int *)data;
//     printf("%d",schedule_id);
//     fflush(stdout);
//     GtkWidget *Parent = GTK_WIDGET(gtk_widget_get_root(GTK_WIDGET(widget)));
//     sqlite3 *db;
//     sqlite3_stmt *stmt;
//     char *err_msg = NULL;
//     int rc = sqlite3_open("Judgment.db",&db);
//         if (rc != SQLITE_OK) {
//         fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
//         sqlite3_close(db);
//         return;
//     }
//     char *sql = g_strdup_printf("SELECT case_schedule.Video_id FROM case_schedule WHERE schedule_id = %d;",schedule_id);
//     sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
//     sqlite3_step(stmt);
//     #if defined(_WIN32) || defined(_WIN64)
//         char *site = g_strdup_printf("start %s",sqlite3_column_text(stmt,0));
//         system(site);
//     #elif defined(__APPLE__)
//         char *site = g_strdup_printf("open %s",sqlite3_column_text(stmt,0));
//         system(site);
//     #else
//         char *site = g_strdup_printf("xdg-open %s",sqlite3_column_text(stmt,0));
//         // send_link_to_browser(sqlite3_column_text(stmt,0));
//         system(site);
//     #endif
//     // g_free(site);
//     g_free(sql);
//     sqlite3_finalize(stmt);
//     sqlite3_close(db);
// }

static void Video_hearing_open(GtkWidget *widget, gpointer data)
{
    int schedule_id = *(int *)data;
    printf("%d", schedule_id);
    fflush(stdout);

    GtkWindow *parent = GTK_WINDOW(gtk_widget_get_root(widget));

    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc = sqlite3_open("Judgment.db", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    char *sql = g_strdup_printf(
        "SELECT Video_id FROM case_schedule WHERE schedule_id = %d;", schedule_id);
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc == SQLITE_OK && sqlite3_step(stmt) == SQLITE_ROW) {
        const char *url = (const char *)sqlite3_column_text(stmt, 0);
        if (url && *url) {
            if (use_broadway) {
                broadway_open_url(url);
            } else {
                GError *error = NULL;
                g_app_info_launch_default_for_uri(url, NULL, &error);
                if (error) {
                    g_printerr("Failed to open URL: %s\n", error->message);
                    g_error_free(error);
                }
            }
        }
    }

    g_free(sql);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}
//
//
GtkWidget *LawyerControlPanel(){
    GtkWidget *grid = gtk_grid_new();
    GtkWidget *label = gtk_label_new("Case ID : , ");
    gtk_widget_set_hexpand(label,TRUE);
    gtk_widget_set_vexpand(label,TRUE);
    GtkWidget *button = gtk_button_new_with_label("View Case Files");
    gtk_widget_set_hexpand(button,TRUE);
    gtk_widget_set_vexpand(button,TRUE);
    g_object_set_data_full(G_OBJECT(grid),"Schedule_id_and_widget",g_memdup2(&(widget_and_id){label,NULL},sizeof(widget_and_id)),g_free);
    g_object_set_data_full(G_OBJECT(button),"Folder",(gpointer)g_strdup("Files"),g_free);
    gtk_grid_attach(GTK_GRID(grid),label,0,0,2,1);
    gtk_grid_attach(GTK_GRID(grid),button,0,1,2,1);
    g_signal_connect(button,"clicked",G_CALLBACK(open_folder),grid);
    button = gtk_button_new_with_label("Open Evidenes");
    gtk_widget_set_hexpand(button,TRUE);
    gtk_widget_set_vexpand(button,TRUE);
    g_object_set_data_full(G_OBJECT(button),"Folder",(gpointer)g_strdup("Evidences"),g_free);
    g_signal_connect(button,"clicked",G_CALLBACK(open_folder),grid);
    gtk_grid_attach(GTK_GRID(grid),button,0,2,2,1);
    GtkWidget *video_button = gtk_button_new_with_label("Video hearing");
    gtk_widget_set_hexpand(video_button,TRUE);
    gtk_widget_set_vexpand(video_button,TRUE);
    g_object_set_data(G_OBJECT(grid),"type",g_memdup2(&(int){2},sizeof(int)));
    g_object_set_data(G_OBJECT(grid), "Video_button", video_button);
    gtk_grid_attach(GTK_GRID(grid),video_button,0,3,2,1);
    gtk_widget_set_sensitive(grid, FALSE);
    return grid;
}
//
/*Lawyer part*/
void Lawyer_dash(GtkWidget *window, gpointer user_data){
    //Login data
    login_data *login = (login_data*)user_data;
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
    gtk_window_set_child(GTK_WINDOW(window), NULL);
    GdkRectangle geometry = GetMonitorSize(window);
    gtk_window_set_resizable(GTK_WINDOW(window), TRUE);
    gtk_window_maximize(GTK_WINDOW(window));
    //
    GtkWidget *container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,2);
    GtkLayoutManager *layout = gtk_constraint_layout_new();
    gtk_widget_set_layout_manager(container,layout);
    gtk_window_set_child(GTK_WINDOW(window), container);
    //
    // Schedule
    //
    int *schedule_id = g_new0(int, 1);
    GtkWidget *box = gtk_grid_new();
    GtkWidget *icon1 = gtk_image_new_from_file("schedule-time-and-date-svgrepo-com.svg");
    GtkWidget *label1 = gtk_label_new("schedule");
    GtkWidget *CaseControlPanel_ = LawyerControlPanel(); 
    GtkWidget **widgets = g_new(GtkWidget *,3);
    GtkWidget *notes = Textbox(schedule_id,NULL,login,"Lawyer Notes");
    char *file = g_strdup("Info.txt");
    GtkWidget *Info = Infobox(schedule_id,file,login);
    widgets[0] = notes;
    widgets[1] = Info;
    widgets[2] = NULL;
    //GtkWidget *HearingTime = clock_();
    gtk_widget_add_css_class(label1,"label");
    gtk_widget_add_css_class(icon1,"label");
    gtk_image_set_pixel_size(GTK_IMAGE(icon1),geometry.width * 0.02);
    gtk_grid_attach(GTK_GRID(box),icon1,0,0,1,1);
    gtk_grid_attach(GTK_GRID(box),label1,1,0,1,1);
    gtk_grid_attach(GTK_GRID(box),Schedule_list(login,schedule_id,CaseControlPanel_,NULL,widgets),0,1,2,1);
    GtkWidget *button = gtk_button_new_with_label("Open Future Schedules");
    GdkRectangle *copy = g_new(GdkRectangle, 1); // allocates memory
    *copy = geometry;                            // copy struct contents
    g_object_set_data_full(G_OBJECT(button), "geometry", copy, g_free);
    g_object_set_data(G_OBJECT(button),"login",login);
    g_object_set_data(G_OBJECT(button),"schedule_id",schedule_id);
    g_object_set_data(G_OBJECT(button),"Control_panel",CaseControlPanel_);
    g_signal_connect(button,"clicked",G_CALLBACK(Future_schedule_panel),widgets);
    gtk_grid_attach(GTK_GRID(box),button,0,2,2,1);
    gtk_box_append(GTK_BOX(container),box);
    //
    gtk_widget_add_css_class(notes,"scroll");
    gtk_box_append(GTK_BOX(container),notes);
    note_box_contraints(layout,CaseControlPanel_,box,notes,geometry);
    //
    gtk_widget_add_css_class(Info,"scroll");
    gtk_box_append(GTK_BOX(container),Info);
    info_box_contraints(layout,CaseControlPanel_,notes,Info,geometry);
    //
    Schedule_grid_contraints(layout,container,box,geometry);
    gtk_box_append(GTK_BOX(container),CaseControlPanel_);
    CaseControlPanel_contraints(layout,box,CaseControlPanel_,geometry);
}
//
//
/*Future schedule panel*/
static void Future_schedule_panel(GtkWidget *widget,gpointer data){
    login_data *login = (login_data *)g_object_get_data(G_OBJECT(widget),"login");
    printf("\n%s\n",login->user_id);
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
    g_object_set_data(G_OBJECT(calendar),"widgets",data);
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
    login_data *login = (login_data *)g_object_get_data(G_OBJECT(data_box),"login");
    int *schedule_id = (int *)g_object_get_data(G_OBJECT(data_box),"schedule_id");
    GtkWidget* Control_panel = (GtkWidget *)g_object_get_data(G_OBJECT(data_box),"Control_panel");
    gtk_box_append(GTK_BOX(data),Schedule_list(login,schedule_id,Control_panel,date,(GtkWidget **)g_object_get_data(G_OBJECT(widget),"widgets")));
}
//
gboolean ensure_directory_exists(const char *path){
    GFile *dir = g_file_new_for_path(path);
    GError *error = NULL;

    // Try to create the directory and its parents.
    gboolean success = g_file_make_directory_with_parents(dir, NULL, &error);

    if (!success) {
        // Check if the error was "already exists", which we consider success.
        if (error->code == G_IO_ERROR_EXISTS) {
            g_clear_error(&error);
            success = TRUE;
        } else {
            // It was a real error (e.g., permissions)
            g_printerr("Error creating directory '%s': %s\n", path, error->message);
            g_clear_error(&error);
            // 'success' is already FALSE
        }
    }

    g_object_unref(dir);
    return success;
}
/*Text boxes*/
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
    char *file_name;
    if (((login_data *)g_object_get_data(G_OBJECT(widget),"login"))->type == 2){
        sqlite3 *db;
        sqlite3_stmt *stmt;
        char *err_msg = NULL;
        int rc = sqlite3_open("Judgment.db",&db);
            if (rc != SQLITE_OK) {
            fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            return;
        }
        char *sql = g_strdup_printf("SELECT CASE WHEN c.Lyr1_ID = %s THEN 'Lyr1_ID' WHEN c.Lyr2_ID = %s THEN 'Lyr2_ID' ELSE 'Not Assigned' END AS lawyer_position FROM case_schedule s JOIN Crime_table c ON s.Case_id = c.Case_ID WHERE s.schedule_id = %d;",((login_data *)g_object_get_data(G_OBJECT(widget),"login"))->user_id,((login_data *)g_object_get_data(G_OBJECT(widget),"login"))->user_id,*schedule_id);
        sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
        sqlite3_step(stmt);
        //
        file_name = g_strdup_printf("%s/Cases/%d/%s.txt",get_executable_folder(),case_id,sqlite3_column_text(stmt,0));
        g_free(sql);
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        g_print("\ntype : %d id: %s\n",((login_data *)g_object_get_data(G_OBJECT(widget),"login"))->type,((login_data *)g_object_get_data(G_OBJECT(widget),"login"))->user_id);
    }
    else if (((login_data *)g_object_get_data(G_OBJECT(widget),"login"))->type == 3){
        file_name = g_strdup_printf("%s/Cases/%d/Hearings/%d.txt",get_executable_folder(),case_id,*schedule_id);
    }
    else{
        file_name = g_strdup_printf("%s/Cases/%d/%s",get_executable_folder(),case_id,(char *)g_object_get_data(G_OBJECT(widget),"file"));
    }char *dir_path = g_path_get_dirname(file_name);
    ensure_directory_exists(dir_path);
    g_print(file_name);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(user_data));
    GtkTextIter start, end;
    gchar *text;

    gtk_text_buffer_get_bounds(buffer, &start, &end);
    text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
    FILE *file = fopen(file_name, "w");
    if (file) {
        fputs(text, file);
        fclose(file);
        g_print("Saved to %s\n",file_name);
    } else {
        g_print("Error: Could not save file.\n");
    }
    g_free(dir_path);
    g_free(text);
}
//
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
    char *file;
    gpointer type = g_object_get_data(G_OBJECT(widget), "type");
    if(!strcmp("Textbox",(char *)type)){
        if (((login_data *)g_object_get_data(G_OBJECT(widget),"login"))->type == 2){
            sqlite3 *db;
            sqlite3_stmt *stmt;
            char *err_msg = NULL;
            int rc = sqlite3_open("Judgment.db",&db);
                if (rc != SQLITE_OK) {
                fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                return;
            }
            char *sql = g_strdup_printf("SELECT CASE WHEN c.Lyr1_ID = %s THEN 'Lyr1_ID' WHEN c.Lyr2_ID = %s THEN 'Lyr2_ID' ELSE 'Not Assigned' END AS lawyer_position FROM case_schedule s JOIN Crime_table c ON s.Case_id = c.Case_ID WHERE s.schedule_id = %d;",((login_data *)g_object_get_data(G_OBJECT(widget),"login"))->user_id,((login_data *)g_object_get_data(G_OBJECT(widget),"login"))->user_id,*schedule_id);
            sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
            sqlite3_step(stmt);
            //
            file = g_strdup_printf("%s/Cases/%d/%s.txt",get_executable_folder(),case_id,sqlite3_column_text(stmt,0));
            g_free(sql);
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            g_print("\ntype : %d id: %s\n",((login_data *)g_object_get_data(G_OBJECT(widget),"login"))->type,((login_data *)g_object_get_data(G_OBJECT(widget),"login"))->user_id);
        }
        else if (((login_data *)g_object_get_data(G_OBJECT(widget),"login"))->type == 3){
            file = g_strdup_printf("%s/Cases/%d/Hearings/%d.txt",get_executable_folder(),case_id,*schedule_id);
        }
        else{
            file = g_strdup_printf("%s/Cases/%d/%s",get_executable_folder(),case_id,(char *)g_object_get_data(G_OBJECT(widget),"file"));
        }
    }
    else{
        file = g_strdup_printf("%s/Cases/%d/%s",get_executable_folder(),case_id,(char *)g_object_get_data(G_OBJECT(widget),"file"));
    }
    g_print(file);
    //
    if (g_file_get_contents(file, &contents, &length, NULL)) {
        gtk_text_buffer_set_text(buffer, contents, length);
        g_free(contents);
    } else {
        g_print("Error: Could not read file.\n");
    }
    g_free(file);
}
//
GtkWidget* Textbox(int *schedule_id,char *file,login_data *login,char *label_name){
    GtkWidget *box;
    GtkWidget *text_view;
    GtkWidget *button;
    GtkWidget *scroll;
    GtkWidget *buttonbox;
    GtkWidget *label;
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    label = gtk_label_new(label_name);
    gtk_widget_set_hexpand(label,TRUE);
    gtk_box_append(GTK_BOX(box), label);
    label = gtk_label_new("Case ID : , ");
    gtk_widget_set_hexpand(label,TRUE);
    gtk_box_append(GTK_BOX(box), label);
    g_object_set_data_full(G_OBJECT(box),"Schedule_id_and_widget",g_memdup2(&(widget_and_id){label,NULL},sizeof(widget_and_id)),g_free);
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
    g_object_set_data_full(G_OBJECT(button),"file",g_strdup_printf("%s",file),g_free);
    g_object_set_data(G_OBJECT(button),"login",login);
    g_object_set_data(G_OBJECT(button),"schedule_id",schedule_id);
    g_signal_connect(button, "clicked", G_CALLBACK(on_save_clicked), text_view);
    gtk_box_append(GTK_BOX(buttonbox),button);
    button = gtk_button_new_with_label("Reload");
    g_object_set_data_full(G_OBJECT(button),"type",g_strdup("Textbox"),g_free);
    g_object_set_data_full(G_OBJECT(button),"file",g_strdup_printf("%s",file),g_free);
    g_object_set_data(G_OBJECT(button),"login",login);
    g_object_set_data(G_OBJECT(button),"schedule_id",schedule_id);
    g_signal_connect(button, "clicked", G_CALLBACK(on_reload_clicked), text_view);
    gtk_box_append(GTK_BOX(buttonbox),button);
    gtk_box_append(GTK_BOX(box), buttonbox);
    // Connect signal
    g_object_set_data_full(G_OBJECT(box),"conn",g_memdup2(&(int){1},sizeof(int)),g_free);
    gtk_widget_set_sensitive(box,FALSE);
    return box;
}
GtkWidget* Infobox(int *schedule_id,char *file,login_data *login){
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
    g_object_set_data_full(G_OBJECT(button),"type",g_strdup("Infobox"),g_free);
    g_object_set_data_full(G_OBJECT(button),"file",g_strdup_printf("%s",file),g_free);
    g_object_set_data(G_OBJECT(button),"login",login);
    g_object_set_data(G_OBJECT(button),"schedule_id",schedule_id);
    g_signal_connect(button, "clicked", G_CALLBACK(on_reload_clicked), text_view);
    gtk_box_append(GTK_BOX(buttonbox),button);
    gtk_box_append(GTK_BOX(box), buttonbox);
    // Connect signal
    gtk_widget_set_sensitive(box,FALSE);
    return box;
}
static void note_box_contraints(GtkLayoutManager *layout,GtkWidget*container,GtkWidget *list,GtkWidget*box,GdkRectangle geometry){
    gtk_constraint_layout_add_constraint(GTK_CONSTRAINT_LAYOUT(layout),
    gtk_constraint_new(
        GTK_CONSTRAINT_TARGET(box),
        GTK_CONSTRAINT_ATTRIBUTE_START,
        GTK_CONSTRAINT_RELATION_EQ,
        GTK_CONSTRAINT_TARGET(list),
        GTK_CONSTRAINT_ATTRIBUTE_END,
        1.0,
        geometry.width *0.015,
        GTK_CONSTRAINT_STRENGTH_REQUIRED
    )
    );
    gtk_constraint_layout_add_constraint(GTK_CONSTRAINT_LAYOUT(layout),
        gtk_constraint_new(
            GTK_CONSTRAINT_TARGET(box),
            GTK_CONSTRAINT_ATTRIBUTE_TOP,
            GTK_CONSTRAINT_RELATION_EQ,
            GTK_CONSTRAINT_TARGET(container),
            GTK_CONSTRAINT_ATTRIBUTE_BOTTOM,
            1.0,
            0,
            GTK_CONSTRAINT_STRENGTH_REQUIRED
        )
    );
    gtk_constraint_layout_add_constraint(GTK_CONSTRAINT_LAYOUT(layout),
        gtk_constraint_new(
            GTK_CONSTRAINT_TARGET(box),
            GTK_CONSTRAINT_ATTRIBUTE_BOTTOM,
            GTK_CONSTRAINT_RELATION_EQ,
            GTK_CONSTRAINT_TARGET(list),
            GTK_CONSTRAINT_ATTRIBUTE_BOTTOM,
            1.0,
            -geometry.height *0.017,
            GTK_CONSTRAINT_STRENGTH_REQUIRED
        )
    );
    // 30% WIDTH (only ONCE)
    gtk_constraint_layout_add_constraint(GTK_CONSTRAINT_LAYOUT(layout),
        gtk_constraint_new(
            GTK_CONSTRAINT_TARGET(box),
            GTK_CONSTRAINT_ATTRIBUTE_WIDTH,
            GTK_CONSTRAINT_RELATION_EQ,
            GTK_CONSTRAINT_TARGET(GTK_WIDGET(gtk_widget_get_root(container))),
            GTK_CONSTRAINT_ATTRIBUTE_WIDTH,
            0.30,
            0,
            GTK_CONSTRAINT_STRENGTH_REQUIRED
        )
    );
    gtk_constraint_layout_add_constraint(GTK_CONSTRAINT_LAYOUT(layout),
        gtk_constraint_new(
            GTK_CONSTRAINT_TARGET(box),
            GTK_CONSTRAINT_ATTRIBUTE_HEIGHT,
            GTK_CONSTRAINT_RELATION_EQ,
            GTK_CONSTRAINT_TARGET(list),
            GTK_CONSTRAINT_ATTRIBUTE_HEIGHT,
            0.69,
            0,
            GTK_CONSTRAINT_STRENGTH_REQUIRED
        )
    );
}
//
static void info_box_contraints(GtkLayoutManager *layout,GtkWidget*container,GtkWidget *notes_box,GtkWidget*info_box,GdkRectangle geometry){
    
    // FIX 1: START constraint: Align info_box START to notes_box END + padding
    gtk_constraint_layout_add_constraint(GTK_CONSTRAINT_LAYOUT(layout),
    gtk_constraint_new(
        GTK_CONSTRAINT_TARGET(info_box), // Target: Case Info Box
        GTK_CONSTRAINT_ATTRIBUTE_START,
        GTK_CONSTRAINT_RELATION_EQ,
        GTK_CONSTRAINT_TARGET(notes_box), // Reference: The Notes Box
        GTK_CONSTRAINT_ATTRIBUTE_END,
        1.0,
        geometry.width *0.015,
        GTK_CONSTRAINT_STRENGTH_REQUIRED
    )
    );

    // END constraint: Align info_box END to container END - padding
    gtk_constraint_layout_add_constraint(GTK_CONSTRAINT_LAYOUT(layout),
    gtk_constraint_new(
        GTK_CONSTRAINT_TARGET(info_box),
        GTK_CONSTRAINT_ATTRIBUTE_END,
        GTK_CONSTRAINT_RELATION_EQ,
        GTK_CONSTRAINT_TARGET(GTK_WIDGET(gtk_widget_get_parent(container))), // Reference: Main horizontal container
        GTK_CONSTRAINT_ATTRIBUTE_END,
        1.0,
        -geometry.width *0.015,
        GTK_CONSTRAINT_STRENGTH_REQUIRED
    )
    );
    
    // TOP constraint (Correct as is)
    gtk_constraint_layout_add_constraint(GTK_CONSTRAINT_LAYOUT(layout),
        gtk_constraint_new(
            GTK_CONSTRAINT_TARGET(info_box),
            GTK_CONSTRAINT_ATTRIBUTE_TOP,
            GTK_CONSTRAINT_RELATION_EQ,
            GTK_CONSTRAINT_TARGET(gtk_widget_get_parent(container)),
            GTK_CONSTRAINT_ATTRIBUTE_TOP,
            1.0,
            geometry.height *0.017,
            GTK_CONSTRAINT_STRENGTH_REQUIRED
        )
    );

    // BOTTOM constraint (Correct as is)
    gtk_constraint_layout_add_constraint(GTK_CONSTRAINT_LAYOUT(layout),
        gtk_constraint_new(
            GTK_CONSTRAINT_TARGET(info_box),
            GTK_CONSTRAINT_ATTRIBUTE_BOTTOM,
            GTK_CONSTRAINT_RELATION_EQ,
            GTK_CONSTRAINT_TARGET(notes_box),
            GTK_CONSTRAINT_ATTRIBUTE_BOTTOM,
            1.0,
            -geometry.height *0.017,
            GTK_CONSTRAINT_STRENGTH_REQUIRED
        )
    );
}
//
/*Stenographer dash*/
void Stenographer_dash(GtkWidget *window, gpointer user_data){
    //Login data
    login_data *login = (login_data*)user_data;
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
    gtk_window_set_child(GTK_WINDOW(window), NULL);
    GdkRectangle geometry = GetMonitorSize(window);
    gtk_window_set_resizable(GTK_WINDOW(window), TRUE);
    gtk_window_maximize(GTK_WINDOW(window));
    //
    GtkWidget *container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,2);
    GtkLayoutManager *layout = gtk_constraint_layout_new();
    gtk_widget_set_layout_manager(container,layout);
    gtk_window_set_child(GTK_WINDOW(window), container);
    //
    // Schedule
    //
    int *schedule_id = g_new0(int, 1);
    GtkWidget *Typing_Area = Textbox(schedule_id,NULL,login,"Stenographer Typing Area");
    gtk_widget_add_css_class(Typing_Area,"scroll");
    gtk_box_append(GTK_BOX(container),Typing_Area);
    GtkWidget *box = gtk_grid_new();
    GtkWidget *icon1 = gtk_image_new_from_file("schedule-time-and-date-svgrepo-com.svg");
    GtkWidget *label1 = gtk_label_new("schedule");
    //GtkWidget *HearingTime = clock_();
    gtk_widget_add_css_class(label1,"label");
    gtk_widget_add_css_class(icon1,"label");
    gtk_image_set_pixel_size(GTK_IMAGE(icon1),geometry.width * 0.02);
    gtk_grid_attach(GTK_GRID(box),icon1,0,0,1,1);
    gtk_grid_attach(GTK_GRID(box),label1,1,0,1,1);
    gtk_grid_attach(GTK_GRID(box),Schedule_list(login,schedule_id,Typing_Area,NULL,NULL),0,1,2,1);
    GtkWidget *button = gtk_button_new_with_label("Open Future Schedules");
    GdkRectangle *copy = g_new(GdkRectangle, 1); // allocates memory
    *copy = geometry;                            // copy struct contents
    g_object_set_data_full(G_OBJECT(button), "geometry", copy, g_free);
    g_object_set_data(G_OBJECT(button),"login",login);
    g_object_set_data(G_OBJECT(button),"schedule_id",schedule_id);
    g_object_set_data(G_OBJECT(button),"Control_panel",Typing_Area);
    GtkWidget **widgets = g_new(GtkWidget *,2);
    g_signal_connect(button,"clicked",G_CALLBACK(Future_schedule_panel),widgets);
    gtk_grid_attach(GTK_GRID(box),button,0,2,2,1);
    gtk_box_append(GTK_BOX(container),box);
    //
    widgets[0] = Typing_Area;
    widgets[1] = NULL;
    //
    info_box_contraints(layout,box,box,Typing_Area,geometry);
    Schedule_grid_contraints(layout,container,box,geometry);
}
//