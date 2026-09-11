// example_client/main.go
//
// This is your "application" that connects to the meet-server with role=client.
// It can:
//   • Request a new meeting (with optional save location + filename)
//   • Close a meeting
//
// Run: go run example_client/main.go
//
// Commands in the terminal:
//   open                              → creates a meeting with default save location
//   open <dir>                        → saves to the given directory, auto filename
//   open <dir> <filename>             → saves to <dir>/<filename>.webm
//   close <id>                        → closes that meeting
//   quit                              → exit
//
// Examples:
//   open /recordings/calls standup-2026-05-02
//   open /tmp/meetings
//   open

package main

import (
	"bufio"
	"encoding/json"
	"fmt"
	"log"
	"os"
	"strings"

	"github.com/gorilla/websocket"
)

type Message struct {
	Type    string          `json:"type"`
	Payload json.RawMessage `json:"payload,omitempty"`
	Error   string          `json:"error,omitempty"`
}

func main() {
	serverURL := "ws://localhost:1755/ws?role=client"
	if len(os.Args) > 1 {
		serverURL = os.Args[1]
	}

	conn, _, err := websocket.DefaultDialer.Dial(serverURL, nil)
	if err != nil {
		log.Fatal("connect error:", err)
	}
	defer conn.Close()

	fmt.Println("✅ Connected to meet-server as [client]")
	fmt.Println("Commands:")
	fmt.Println("  open                       — create meeting (default save location)")
	fmt.Println("  open <dir>                 — save to <dir>, auto filename")
	fmt.Println("  open <dir> <filename>      — save to <dir>/<filename>.webm")
	fmt.Println("  close <room_id>            — close a meeting")
	fmt.Println("  quit                       — exit")
	fmt.Println(strings.Repeat("─", 55))

	// ── Receive goroutine ───────────────────────────────────────────────────
	go func() {
		for {
			_, data, err := conn.ReadMessage()
			if err != nil {
				log.Println("connection closed:", err)
				os.Exit(0)
			}
			var msg Message
			if err := json.Unmarshal(data, &msg); err != nil {
				log.Println("bad JSON from server:", string(data))
				continue
			}
			switch msg.Type {
			case "meeting_link":
				var p struct {
					RoomID   string `json:"room_id"`
					URL      string `json:"url"`
					FilePath string `json:"file_path"`
				}
				_ = json.Unmarshal(msg.Payload, &p)
				fmt.Printf("\n🔗 Meeting created!\n")
				fmt.Printf("   Room ID   : %s\n", p.RoomID)
				fmt.Printf("   URL       : %s\n", p.URL)
				fmt.Printf("   Recording : %s\n\n> ", p.FilePath)
			case "meeting_closed":
				var p map[string]string
				_ = json.Unmarshal(msg.Payload, &p)
				fmt.Printf("\n🔴 Room %s closed\n\n> ", p["room_id"])
			case "error":
				fmt.Printf("\n❌ Error: %s\n\n> ", msg.Error)
			default:
				fmt.Printf("\n📨 [%s] %s\n\n> ", msg.Type, string(msg.Payload))
			}
		}
	}()

	// ── Send loop ───────────────────────────────────────────────────────────
	scanner := bufio.NewScanner(os.Stdin)
	fmt.Print("> ")
	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		parts := strings.Fields(line)
		if len(parts) == 0 {
			fmt.Print("> ")
			continue
		}

		switch parts[0] {
		case "open":
			// Build the OpenMeetingPayload.
			// Fields are optional — omit them to use server defaults.
			type openPayload struct {
				Dir      string `json:"dir,omitempty"`
				FileName string `json:"filename,omitempty"`
			}
			p := openPayload{}
			if len(parts) >= 2 {
				p.Dir = parts[1]
			}
			if len(parts) >= 3 {
				p.FileName = parts[2]
			}
			payload, _ := json.Marshal(p)
			send(conn, Message{Type: "open_meeting", Payload: payload})

		case "close":
			if len(parts) < 2 {
				fmt.Println("usage: close <room_id>")
			} else {
				payload, _ := json.Marshal(map[string]string{"room_id": parts[1]})
				send(conn, Message{Type: "close_meeting", Payload: payload})
			}

		case "quit", "exit":
			conn.Close()
			return

		default:
			fmt.Println("unknown command:", parts[0])
		}
		fmt.Print("> ")
	}
}

func send(conn *websocket.Conn, msg Message) {
	data, _ := json.Marshal(msg)
	if err := conn.WriteMessage(websocket.TextMessage, data); err != nil {
		log.Println("send error:", err)
	}
}
