#include <libsoup/soup.h>
#include <gtk/gtk.h>
#include <json-glib/json-glib.h>

typedef struct {
    GMainLoop *loop;
    SoupWebsocketConnection *connection;
    char *url;
    char *roomid;
} AppData;

// --- JSON Helpers ---

void send_json_message(SoupWebsocketConnection *conn, const char *type, JsonNode *payload_node) {
    JsonBuilder *builder = json_builder_new();
    json_builder_begin_object(builder);
    json_builder_set_member_name(builder, "type");
    json_builder_add_string_value(builder, type);
    
    if (payload_node) {
        json_builder_set_member_name(builder, "payload");
        json_builder_add_value(builder, json_node_copy(payload_node));
    }
    
    json_builder_end_object(builder);

    JsonGenerator *gen = json_generator_new();
    json_generator_set_root(gen, json_builder_get_root(builder));
    size_t len;
    char *output = json_generator_to_data(gen, &len);

    soup_websocket_connection_send_text(conn, output);

    g_free(output);
    g_object_unref(gen);
    g_object_unref(builder);
}

// --- WebSocket Handlers ---

void web_handler_message(SoupWebsocketConnection *connection, SoupWebsocketDataType type, GBytes *message, gpointer user_data) {
    AppData *app = (AppData *)user_data;
    if (type != SOUP_WEBSOCKET_DATA_TEXT) return;

    gsize size;
    const char *data = g_bytes_get_data(message, &size);
    
    JsonParser *parser = json_parser_new();
    if (json_parser_load_from_data(parser, data, size, NULL)) {
        JsonObject *root = json_node_get_object(json_parser_get_root(parser));
        const char *msg_type = json_object_get_string_member(root, "type");

        if (g_strcmp0(msg_type, "meeting_link") == 0) {
            JsonObject *payload = json_object_get_object_member(root, "payload");
            g_print("\n🔗 Meeting created!\n   Room ID: %s\n   URL: %s\n> ", 
                    json_object_get_string_member(payload, "room_id"),
                    json_object_get_string_member(payload, "url"));
            app->url = g_strdup_printf(json_object_get_string_member(payload, "url"));
            app->roomid = g_strdup_printf(json_object_get_string_member(payload, "room_id"));
        } else if (g_strcmp0(msg_type, "meeting_closed") == 0) {
            JsonObject *payload = json_object_get_object_member(root, "payload");
            g_print("\n🔴 Room %s closed\n> ", json_object_get_string_member(payload, "room_id"));
        } else if (g_strcmp0(msg_type, "error") == 0) {
            g_print("\n❌ Error: %s\n> ", json_object_get_string_member(root, "error"));
        }
    }
    g_object_unref(parser);
}

// --- CLI Input Handler (The "Go-style" loop) ---

gboolean on_stdin_input(GIOChannel *source, GIOCondition condition, gpointer user_data) {
    AppData *app = (AppData *)user_data;
    char *line = NULL;
    gsize length;
    g_io_channel_read_line(source, &line, &length, NULL, NULL);

    if (!line || !app->connection) return TRUE;

    char **parts = g_strsplit(g_strstrip(line), " ", -1);
    if (parts[0]) {
        if (g_strcmp0(parts[0], "open") == 0) {
            JsonBuilder *b = json_builder_new();
            json_builder_begin_object(b);
            if (parts[1]) json_builder_set_member_name(b, "dir"), json_builder_add_string_value(b, parts[1]);
            if (parts[1] && parts[2]) json_builder_set_member_name(b, "filename"), json_builder_add_string_value(b, parts[2]);
            json_builder_end_object(b);
            
            send_json_message(app->connection, "open_meeting", json_builder_get_root(b));
            g_object_unref(b);
        } 
        else if (g_strcmp0(parts[0], "close") == 0 && parts[1]) {
            JsonBuilder *b = json_builder_new();
            json_builder_begin_object(b);
            json_builder_set_member_name(b, "room_id");
            json_builder_add_string_value(b, parts[1]);
            json_builder_end_object(b);

            send_json_message(app->connection, "close_meeting", json_builder_get_root(b));
            g_object_unref(b);
        }
    }

    g_strfreev(parts);
    g_free(line);
    g_print("> ");
    return TRUE;
}

// --- Lifecycle ---

void web_handler(GObject *session, GAsyncResult *res, gpointer user_data) {
    AppData *app = (AppData *)user_data;
    GError *error = NULL;
    app->connection = soup_session_websocket_connect_finish(SOUP_SESSION(session), res, &error);
    
    if (error) {
        g_printerr("Connect Error: %s\n", error->message);
        return;
    }

    g_print("✅ Connected to meet-server\nCommands: open, close <id>, quit\n> ");
    g_signal_connect(app->connection, "message", G_CALLBACK(web_handler_message), app);
}