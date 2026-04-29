package main

import (
	"bufio"
	"fmt"
	"log"
	"net/http"
	"os"
	"sync"

	"github.com/gorilla/websocket"
)

// Consts for easiy usage :::{
const (
	browser_to_server = "Br2Sr"
	server_to_browser = "Sr2Br"
)

// }

// Structure :::{
type connection struct {
	browser *websocket.Conn
	server  *websocket.Conn
	mu      sync.Mutex
}

// }

// Global Variables :::{
var connection_global connection

var upgrader = websocket.Upgrader{
	CheckOrigin: func(r *http.Request) bool { return true },
}

// }

// Relay Function :::{
func relay(connection_ *connection, direction string, msg []byte) {
	switch direction {
	case browser_to_server:
		{
			if connection_.server == nil {
				log.Println("server not connected ! ")
				return
			} else {
				connection_.mu.Lock()
				defer connection_.mu.Unlock()
				connection_.server.WriteMessage(websocket.TextMessage, msg)
			}
		}
	case server_to_browser:
		{
			if connection_.browser == nil {
				log.Println("server not connected ! ")
				return
			} else {
				connection_.mu.Lock()
				defer connection_.mu.Unlock()
				connection_.browser.WriteMessage(websocket.TextMessage, msg)
			}
		}
	default:
		{
			log.Println("Incorrect direction !")
		}
	}
}

// }

// WS function :::{
func handler(reponse http.ResponseWriter, request *http.Request) {

	// get role (from /(handle string)?role=(This_role)) :::{
	var role = request.URL.Query().Get("role")
	log.Print("Connection role : ", role)

	// }

	// Internal variables :::{
	var error_ error
	var connection_ *websocket.Conn

	// }

	// set connection type :::{
	switch role {
	case browser_to_server:
		{
			connection_global.mu.Lock()
			connection_global.browser, error_ = upgrader.Upgrade(reponse, request, nil)
			if error_ != nil {
				log.Println("HTTP connection Upgrade Error : ", error_)

			}
			defer connection_global.browser.Close()
			connection_ = connection_global.browser
			connection_global.mu.Unlock()
		}
	case server_to_browser:
		{
			connection_global.mu.Lock()
			connection_global.server, error_ = upgrader.Upgrade(reponse, request, nil)
			if error_ != nil {
				log.Println("HTTP connection Upgrade Error : ", error_)

			}
			defer connection_global.server.Close()
			connection_ = connection_global.server
			connection_global.mu.Unlock()
		}
	default:
		{
			log.Println("Unknown Role ! : ", role)
			return
		}
	}

	// }

	// Ws message loop :::{
	for {
		var client_message_type, client_message, error_ = connection_.ReadMessage()
		if error_ != nil {
			log.Println("Client Connection Dropped ! = ", error_)
			break
		}
		log.Println("Client message = ", client_message)
		connection_.WriteMessage(client_message_type, client_message)
		relay(&connection_global, role, client_message)
	}

	// }

	connection_global.mu.Lock()
	connection_ = nil
	switch role {
	case browser_to_server:
		{
			connection_global.browser = nil
		}
	case server_to_browser:
		{
			connection_global.server = nil
		}
	default:
		{
			log.Println("Incorrect Role ! :", role)
		}
	}
	connection_global.mu.Unlock()
}

// }

func main() {
	go func() {
		http.HandleFunc("/test", handler)
		log.Println("gg")
		log.Fatal(http.ListenAndServe(":1755", nil))
	}()

	scanner := bufio.NewScanner(os.Stdin)
	for scanner.Scan() {
		var input = scanner.Text()
		fmt.Printf("input: %v\n", input)

		err := scanner.Err()
		if err != nil {
			fmt.Println("Error reading input:", err)
		} else if scanner.Text() == "start" {
			connection_global.browser.WriteMessage(websocket.TextMessage, []byte(`{"type":"start"}`))
		} else if scanner.Text() == "stop" {
			connection_global.browser.WriteMessage(websocket.TextMessage, []byte(`{"type":"stop"}`))
		} else if scanner.Text() == "answer" {
			connection_global.browser.WriteMessage(websocket.TextMessage, []byte(`{"type":"answer"}`))
		}
	}
}
