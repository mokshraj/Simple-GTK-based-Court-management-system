package api

import (
	"encoding/json"
	"fmt"
	"log"
	"net/http"
	"sync"

	"github.com/google/uuid"
	"github.com/gorilla/websocket"

	"court-server/internal/meet/room"
)

// ────────────────────────────────────────────────────────────────────────────────
// Handler
// ────────────────────────────────────────────────────────────────────────────────

type Handler struct {
	rm      *room.Manager
	baseURL string // e.g. "http://localhost:1755"
}

func NewHandler(rm *room.Manager) *Handler {
	return &Handler{
		rm:      rm,
		baseURL: "https://[200:53ca:1b5e:353:20db:721:f689:2d8]:1755",
	}
}

var upgrader = websocket.Upgrader{
	CheckOrigin: func(r *http.Request) bool { return true },
}

// ────────────────────────────────────────────────────────────────────────────────
// /rooms  (REST – list active rooms)
// ────────────────────────────────────────────────────────────────────────────────

func (h *Handler) HandleRooms(w http.ResponseWriter, r *http.Request) {
	ids := h.rm.ListRooms()
	w.Header().Set("Content-Type", "application/json")
	_ = json.NewEncoder(w).Encode(map[string]interface{}{"rooms": ids})
}

// ────────────────────────────────────────────────────────────────────────────────
// /ws  (WebSocket – all roles)
// ────────────────────────────────────────────────────────────────────────────────
//
// Query parameters:
//
//	role   = "client"  | "browser"
//	room   = <room_id>   (required for role=browser)
//
// Role semantics:
//
//	client  → your backend application. It can:
//	            • send {type:"open_meeting", payload:{dir:"...", filename:"..."}}
//	                → server creates room + returns meeting_link with file_path
//	            • send {type:"close_meeting", payload:{room_id:"..."}}
//	                → server closes room
//	          It receives:
//	            • {type:"meeting_link", payload:{room_id, url, file_path}}
//
//	browser → a WebRTC browser peer. Must supply ?room=<id>.
//	          Participates in WebRTC signaling (offer/answer/candidate relay).
func (h *Handler) HandleWS(w http.ResponseWriter, r *http.Request) {
	role := r.URL.Query().Get("role")
	if role == "" {
		http.Error(w, "missing ?role=", http.StatusBadRequest)
		return
	}

	conn, err := upgrader.Upgrade(w, r, nil)
	if err != nil {
		log.Println("upgrade error:", err)
		return
	}

	switch role {
	case "client":
		h.handleClientRole(conn)
	case "browser":
		roomID := r.URL.Query().Get("room")
		if roomID == "" {
			writeMsg(conn, nil, room.Message{Type: room.MsgError, Error: "missing ?room="})
			conn.Close()
			return
		}
		h.handleBrowserRole(conn, roomID)
	default:
		writeMsg(conn, nil, room.Message{Type: room.MsgError, Error: fmt.Sprintf("unknown role: %s", role)})
		conn.Close()
	}
}

// ────────────────────────────────────────────────────────────────────────────────
// Client role handler
// ────────────────────────────────────────────────────────────────────────────────

func (h *Handler) handleClientRole(conn *websocket.Conn) {
	defer conn.Close()

	peerID := uuid.New().String()
	log.Printf("client connected  peer=%s", peerID)

	var mu sync.Mutex
	send := func(msg room.Message) error {
		return writeMsg(conn, &mu, msg)
	}

	for {
		_, raw, err := conn.ReadMessage()
		if err != nil {
			log.Printf("client %s disconnected: %v", peerID, err)
			return
		}

		var msg room.Message
		if err := json.Unmarshal(raw, &msg); err != nil {
			_ = send(room.Message{Type: room.MsgError, Error: "invalid JSON"})
			continue
		}

		switch msg.Type {

		case room.MsgOpenMeeting:
			// Extract optional recording preferences from the payload.
			// Both fields are optional; empty strings → server defaults.
			//
			// Expected payload:
			//   {"dir": "/path/to/save", "filename": "my-meeting"}
			var recOpts room.OpenMeetingPayload
			if msg.Payload != nil {
				recOpts, _ = extractPayload[room.OpenMeetingPayload](msg.Payload)
			}

			r, err := h.rm.CreateRoom(recOpts.Dir, recOpts.FileName)
			if err != nil {
				_ = send(room.Message{Type: room.MsgError, Error: err.Error()})
				continue
			}

			meetingURL := fmt.Sprintf("%s/?room=%s", h.baseURL, r.ID)
			_ = send(room.Message{
				Type: room.MsgMeetingLink,
				Payload: room.MeetingLinkPayload{
					RoomID:   r.ID,
					URL:      meetingURL,
					FilePath: r.RecordingPath(),
				},
			})
			log.Printf("client %s created room %s → %s  (recording: %s)",
				peerID, r.ID, meetingURL, r.RecordingPath())

		case "close_meeting":
			payload, _ := extractPayload[map[string]string](msg.Payload)
			roomID, _ := payload["room_id"]
			if roomID == "" {
				_ = send(room.Message{Type: room.MsgError, Error: "missing room_id in close_meeting"})
				continue
			}
			h.rm.CloseRoom(roomID)
			_ = send(room.Message{Type: "meeting_closed", Payload: map[string]string{"room_id": roomID}})

		default:
			_ = send(room.Message{Type: room.MsgError, Error: fmt.Sprintf("unknown message type: %s", msg.Type)})
		}
	}
}

// ────────────────────────────────────────────────────────────────────────────────
// Browser role handler (WebRTC signaling relay)
// ────────────────────────────────────────────────────────────────────────────────

func (h *Handler) handleBrowserRole(conn *websocket.Conn, roomID string) {
	defer conn.Close()

	rm := h.rm.GetRoom(roomID)
	if rm == nil {
		writeMsg(conn, nil, room.Message{Type: room.MsgError, Error: fmt.Sprintf("room %s not found", roomID)})
		return
	}

	peerID := uuid.New().String()
	log.Printf("browser joined  peer=%s  room=%s", peerID, roomID)

	var mu sync.Mutex
	peer := &room.Peer{
		ID:     peerID,
		Role:   "browser",
		RoomID: roomID,
		Send: func(msg room.Message) error {
			return writeMsg(conn, &mu, msg)
		},
	}

	rm.AddPeer(peer)
	defer rm.RemovePeer(peerID)

	_ = peer.Send(room.Message{
		Type:    "welcome",
		Payload: map[string]interface{}{"peer_id": peerID, "room_id": roomID, "peers": rm.PeerIDs()},
	})

	for {
		_, raw, err := conn.ReadMessage()
		if err != nil {
			log.Printf("browser %s disconnected from room %s: %v", peerID, roomID, err)
			return
		}

		var msg room.Message
		if err := json.Unmarshal(raw, &msg); err != nil {
			_ = peer.Send(room.Message{Type: room.MsgError, Error: "invalid JSON"})
			continue
		}

		switch msg.Type {

		case room.MsgOffer, room.MsgAnswer, room.MsgCandidate:
			payload, err := extractPayload[room.SignalPayload](msg.Payload)
			if err != nil {
				rm.Broadcast(msg, peerID)
				continue
			}
			payload.From = peerID
			msg.Payload = payload

			if payload.To != "" {
				if err := rm.SendToPeer(payload.To, msg); err != nil {
					log.Printf("relay error: %v", err)
				}
			} else {
				rm.Broadcast(msg, peerID)
			}

		default:
			rm.Broadcast(msg, peerID)
		}
	}
}

// ────────────────────────────────────────────────────────────────────────────────
// Helpers
// ────────────────────────────────────────────────────────────────────────────────

func writeMsg(conn *websocket.Conn, mu *sync.Mutex, msg room.Message) error {
	data, err := json.Marshal(msg)
	if err != nil {
		return err
	}
	if mu != nil {
		mu.Lock()
		defer mu.Unlock()
	}
	return conn.WriteMessage(websocket.TextMessage, data)
}

func extractPayload[T any](raw interface{}) (T, error) {
	var result T
	b, err := json.Marshal(raw)
	if err != nil {
		return result, err
	}
	err = json.Unmarshal(b, &result)
	return result, err
}
