#include <libsoup/soup.h>
// Websocket handler functions :::{

    // Function Declarartions :::{
        void web_handler_on_close(SoupWebsocketConnection*,gpointer);
        void web_handler(GObject*,GAsyncResult*,gpointer);
        void web_handler_message(SoupWebsocketConnection*,SoupWebsocketDataType,GBytes*,gpointer);
    // }

    // Function Construction :::{

        // Websocket Connection Function :::{
            void web_handler(GObject *session ,GAsyncResult *async_result,gpointer loop){
                GError *error = NULL;
                SoupWebsocketConnection *connection = soup_session_websocket_connect_finish(SOUP_SESSION(session),async_result,&error);
                if(error){
                    g_printerr("Websocket connection Error : \n%s\n%d,",error->message,error->code);
                    g_error_free(error);
                    g_main_loop_quit((GMainLoop *)loop);
                }
                else{
                    g_print("\nWebsocket connected !");
                    soup_websocket_connection_send_text(connection,"hello");
                    g_signal_connect(connection,"closed",G_CALLBACK(web_handler_on_close),loop);
                    g_signal_connect(connection,"message",G_CALLBACK(web_handler_message),NULL);
                    // soup_websocket_connection_close(connection,SOUP_WEBSOCKET_CLOSE_NORMAL,NULL);
                }
            }
        // }

        // Websocket Message Handling function :::{
            void web_handler_message(SoupWebsocketConnection *connection,SoupWebsocketDataType type,GBytes* message,gpointer data){
                soup_websocket_connection_close(connection,SOUP_WEBSOCKET_CLOSE_NORMAL,NULL);
            }
        // }

        // Websocket On close Gmainloop closing function :::{
            void web_handler_on_close(SoupWebsocketConnection *connection,gpointer loop){
                g_print("\nWebSocket closed !");
                g_object_unref(connection);
                g_main_loop_quit((GMainLoop*)loop);
            }
        // }

    // }

// }

int main(){
    GMainLoop *loop =  g_main_loop_new(NULL,FALSE);
    SoupSession *session = soup_session_new();
    SoupMessage *session_message = soup_message_new("GET","ws://localhost:1755/test?role=Sr2Br");
    // GCancellable *cancel;
    soup_session_websocket_connect_async(session,session_message,NULL,NULL,G_PRIORITY_DEFAULT,NULL,web_handler,loop);
    g_main_loop_run(loop);
    // GInputStream *msg = g_memory_input_stream_new_from_data("msg",-1,NULL);
    // soup_message_set_request_body(session_message,"msg",msg,-1);
    g_object_unref(session);
    g_object_unref(session_message);
    // g_object_unref(msg);
    // g_object_unref(cancel);
    g_main_loop_unref(loop);
    return 0;
}