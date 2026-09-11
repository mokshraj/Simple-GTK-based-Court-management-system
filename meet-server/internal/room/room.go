package room

import (
	"fmt"
	"log"
	"sync"
	"time"

	"github.com/google/uuid"
)

// ────────────────────────────────────────────────────────────────────────────────
// Constants
// ────────────────────────────────────────────────────────────────────────────────

const (
	// EmptyRoomTimeout is how long a room stays alive after the last peer leaves.
	// The room (and its recording) is automatically closed after this duration.
	EmptyRoomTimeout = 5 * time.Minute

	// Message types (JSON field "type")
	MsgOpenMeeting = "open_meeting"
	MsgJoinMeeting = "join_meeting"
	MsgOffer       = "offer"
	MsgAnswer      = "answer"
	MsgCandidate   = "candidate"
	MsgMeetingLink = "meeting_link"
	MsgPeerJoined  = "peer_joined"
	MsgPeerLeft    = "peer_left"
	MsgError       = "error"
	MsgRoomClosed  = "room_closed"
)

// ────────────────────────────────────────────────────────────────────────────────
// Message envelope
// ────────────────────────────────────────────────────────────────────────────────

type Message struct {
	Type    string      `json:"type"`
	Payload interface{} `json:"payload,omitempty"`
	Error   string      `json:"error,omitempty"`
}

// OpenMeetingPayload is sent by the client to create a new room.
// Both fields are optional; the server uses safe defaults when omitted.
//
//	{"type":"open_meeting","payload":{"dir":"/recordings","filename":"standup"}}
type OpenMeetingPayload struct {
	Dir      string `json:"dir"`      // directory to save file in
	FileName string `json:"filename"` // base name (.mp4 appended automatically)
}

// MeetingLinkPayload is returned to the client after a room is created.
type MeetingLinkPayload struct {
	RoomID   string `json:"room_id"`
	URL      string `json:"url"`
	FilePath string `json:"file_path"` // absolute path of the recording file
}

type SignalPayload struct {
	From      string      `json:"from"`
	To        string      `json:"to"`
	SDP       string      `json:"sdp,omitempty"`
	Candidate interface{} `json:"candidate,omitempty"`
}

type PeerPayload struct {
	PeerID string `json:"peer_id"`
}

// ────────────────────────────────────────────────────────────────────────────────
// Peer
// ────────────────────────────────────────────────────────────────────────────────

type SendFunc func(msg Message) error

type Peer struct {
	ID       string
	Role     string
	RoomID   string
	Send     SendFunc
	JoinedAt time.Time
}

// ────────────────────────────────────────────────────────────────────────────────
// Room
// ────────────────────────────────────────────────────────────────────────────────

type Room struct {
	ID        string
	CreatedAt time.Time
	OwnerID   string

	mu          sync.RWMutex
	peers       map[string]*Peer
	recorder    *Recorder
	closeTimer  *time.Timer         // fires after EmptyRoomTimeout when room is empty
	onAutoClose func(roomID string) // wired up by Manager.CreateRoom
}

func newRoom(id string, recorder *Recorder) *Room {
	return &Room{
		ID:        id,
		CreatedAt: time.Now(),
		peers:     make(map[string]*Peer),
		recorder:  recorder,
	}
}

// AddPeer registers a peer and cancels any pending auto-close timer.
func (r *Room) AddPeer(p *Peer) {
	r.mu.Lock()
	// A new peer arrived — cancel the empty-room countdown
	if r.closeTimer != nil {
		r.closeTimer.Stop()
		r.closeTimer = nil
		log.Printf("room %s: auto-close timer cancelled (peer %s joined)", r.ID, p.ID)
	}
	r.peers[p.ID] = p
	r.mu.Unlock()

	r.Broadcast(Message{
		Type:    MsgPeerJoined,
		Payload: PeerPayload{PeerID: p.ID},
	}, p.ID)
}

// RemovePeer unregisters a peer. If the room becomes empty it stops the
// recorder and starts the EmptyRoomTimeout countdown; the Manager will close
// the room when the timer fires.
func (r *Room) RemovePeer(peerID string) {
	r.mu.Lock()
	delete(r.peers, peerID)
	remaining := len(r.peers)

	if remaining == 0 {
		// Stop the recorder as soon as the last peer leaves so the MP4 is
		// finalised promptly; the room entry stays alive for the timeout period
		// in case someone reconnects.
		if r.recorder != nil {
			r.recorder.Stop()
		}
		// Start auto-close countdown
		onClose := r.onAutoClose
		roomID := r.ID
		r.closeTimer = time.AfterFunc(EmptyRoomTimeout, func() {
			log.Printf("room %s: auto-close after %s of inactivity", roomID, EmptyRoomTimeout)
			if onClose != nil {
				onClose(roomID)
			}
		})
		log.Printf("room %s: empty — auto-close in %s", r.ID, EmptyRoomTimeout)
	}
	r.mu.Unlock()

	r.Broadcast(Message{
		Type:    MsgPeerLeft,
		Payload: PeerPayload{PeerID: peerID},
	}, "")
}

// Broadcast sends msg to all peers except excludePeerID.
// Pass "" to send to everyone.
func (r *Room) Broadcast(msg Message, excludePeerID string) {
	r.mu.RLock()
	defer r.mu.RUnlock()
	for id, p := range r.peers {
		if id == excludePeerID {
			continue
		}
		if err := p.Send(msg); err != nil {
			log.Printf("room %s: send to peer %s failed: %v", r.ID, id, err)
		}
	}
}

// SendToPeer delivers a message to a specific peer.
func (r *Room) SendToPeer(peerID string, msg Message) error {
	r.mu.RLock()
	p, ok := r.peers[peerID]
	r.mu.RUnlock()
	if !ok {
		return fmt.Errorf("peer %s not in room %s", peerID, r.ID)
	}
	return p.Send(msg)
}

func (r *Room) PeerIDs() []string {
	r.mu.RLock()
	defer r.mu.RUnlock()
	ids := make([]string, 0, len(r.peers))
	for id := range r.peers {
		ids = append(ids, id)
	}
	return ids
}

// RecordingPath returns the file path of the recording, or "" if none.
func (r *Room) RecordingPath() string {
	if r.recorder == nil {
		return ""
	}
	return r.recorder.FilePath
}

// cancelAutoClose stops the auto-close timer if it's running.
// Must be called with r.mu held or when no races can occur.
func (r *Room) cancelAutoClose() {
	if r.closeTimer != nil {
		r.closeTimer.Stop()
		r.closeTimer = nil
	}
}

// ────────────────────────────────────────────────────────────────────────────────
// Manager
// ────────────────────────────────────────────────────────────────────────────────

type Manager struct {
	mu    sync.RWMutex
	rooms map[string]*Room
}

func NewManager() *Manager {
	return &Manager{rooms: make(map[string]*Room)}
}

// CreateRoom creates a new room, starts its GStreamer recorder, and wires the
// auto-close callback so an empty room is cleaned up after EmptyRoomTimeout.
func (m *Manager) CreateRoom(outputDir, fileName string) (*Room, error) {
	id := uuid.New().String()

	rec, err := NewRecorder(outputDir, fileName, id)
	if err != nil {
		return nil, fmt.Errorf("recorder for room %s: %w", id, err)
	}

	r := newRoom(id, rec)
	// Wire auto-close: when the timer fires it calls Manager.CloseRoom
	r.onAutoClose = func(roomID string) {
		m.CloseRoom(roomID)
	}

	m.mu.Lock()
	m.rooms[id] = r
	m.mu.Unlock()

	log.Printf("room %s created  recording -> %s", id, rec.FilePath)
	return r, nil
}

// GetRoom returns the room or nil if not found.
func (m *Manager) GetRoom(id string) *Room {
	m.mu.RLock()
	defer m.mu.RUnlock()
	return m.rooms[id]
}

// CloseRoom cancels any pending auto-close timer, stops recording, notifies
// remaining peers, and removes the room from the manager.
func (m *Manager) CloseRoom(id string) {
	m.mu.Lock()
	r, ok := m.rooms[id]
	if ok {
		delete(m.rooms, id)
	}
	m.mu.Unlock()

	if !ok {
		return
	}

	r.mu.Lock()
	r.cancelAutoClose()
	r.mu.Unlock()

	if r.recorder != nil {
		r.recorder.Stop()
	}
	r.Broadcast(Message{Type: MsgRoomClosed}, "")
	log.Printf("room %s closed", id)
}

// ListRooms returns a snapshot of active room IDs.
func (m *Manager) ListRooms() []string {
	m.mu.RLock()
	defer m.mu.RUnlock()
	ids := make([]string, 0, len(m.rooms))
	for id := range m.rooms {
		ids = append(ids, id)
	}
	return ids
}
