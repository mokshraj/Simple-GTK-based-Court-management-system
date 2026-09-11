package portal

import (
	"encoding/json"
	"log"
	"sync"

	"github.com/gorilla/websocket"
)

// wsMessage is the JSON envelope sent over the portal WebSocket.
type wsMessage struct {
	Type string `json:"type"`
	URL  string `json:"url,omitempty"`
}

// WSHub maintains a registry of active WebSocket connections, keyed by session ID.
// It is safe for concurrent use.
type WSHub struct {
	mu             sync.RWMutex
	sessions       map[int][]*websocket.Conn // one browser tab per session
	OnSessionClose func(sessionID int)       // called when all WS connections for a session close
}

func NewWSHub() *WSHub {
	return &WSHub{sessions: make(map[int][]*websocket.Conn)}
}

// Register adds a WebSocket connection to the session.
func (h *WSHub) Register(sessionID int, conn *websocket.Conn) {
	h.mu.Lock()
	defer h.mu.Unlock()
	h.sessions[sessionID] = append(h.sessions[sessionID], conn)
	log.Printf("portal: ws registered session=%d connections=%d", sessionID, len(h.sessions[sessionID]))
}

// Unregister removes a WebSocket connection from the session.
func (h *WSHub) Unregister(sessionID int, conn *websocket.Conn) {
	h.mu.Lock()
	conns, ok := h.sessions[sessionID]
	if !ok {
		h.mu.Unlock()
		return
	}
	filtered := make([]*websocket.Conn, 0, len(conns))
	for _, c := range conns {
		if c != conn {
			filtered = append(filtered, c)
		}
	}
	if len(filtered) == 0 {
		delete(h.sessions, sessionID)
	} else {
		h.sessions[sessionID] = filtered
	}
	h.mu.Unlock()
	log.Printf("portal: ws unregistered session=%d connections=%d", sessionID, len(filtered))

	// If no more connections for this session, trigger cleanup
	if len(filtered) == 0 && h.OnSessionClose != nil {
		h.OnSessionClose(sessionID)
	}
}

// BroadcastToSession sends a JSON message to all connections for a session.
func (h *WSHub) BroadcastToSession(sessionID int, msg wsMessage) {
	h.mu.RLock()
	conns, ok := h.sessions[sessionID]
	h.mu.RUnlock()
	if !ok {
		return
	}
	data, err := json.Marshal(msg)
	if err != nil {
		log.Printf("portal: ws marshal error: %v", err)
		return
	}
	for _, conn := range conns {
		if err := conn.WriteMessage(websocket.TextMessage, data); err != nil {
			log.Printf("portal: ws write error session=%d: %v", sessionID, err)
		}
	}
}

// CloseAll closes every connection and clears the registry.
func (h *WSHub) CloseAll() {
	h.mu.Lock()
	defer h.mu.Unlock()
	for sessionID, conns := range h.sessions {
		for _, conn := range conns {
			_ = conn.WriteMessage(websocket.CloseMessage, websocket.FormatCloseMessage(websocket.CloseNormalClosure, ""))
			_ = conn.Close()
		}
		delete(h.sessions, sessionID)
		log.Printf("portal: ws closed session=%d connections=%d", sessionID, len(conns))
	}
}
