package portal

import (
	"fmt"
	"sync"
)

// MaxSessions caps the number of concurrent Broadway tabs (rotates 1..N).
const MaxSessions = 50

// SessionManager hands out monotonically increasing session IDs (1..MaxSessions).
// It mirrors the original counter from Go_backend/main.go but is now reusable
// from both the HTTP handlers and the TUI.
type SessionManager struct {
	mu           sync.Mutex
	sessionCount int
}

func NewSessionManager() *SessionManager { return &SessionManager{} }

// GetNextID returns the next available session ID in [1, MaxSessions], skipping
// IDs that are already in use by live sessions. It returns an error if all
// slots are full.
func (sm *SessionManager) GetNextID(reg *SessionRegistry) (int, error) {
	sm.mu.Lock()
	defer sm.mu.Unlock()
	// Start from sessionCount+1 to avoid reusing the same ID twice in a row
	// when sessions are rapidly opened/closed.
	startID := (sm.sessionCount % MaxSessions) + 1
	for i := 0; i < MaxSessions; i++ {
		id := (startID+i-1)%MaxSessions + 1
		if _, live := reg.Get(id); !live {
			sm.sessionCount = id - 1 // track last allocated for next time
			return id, nil
		}
	}
	return 0, fmt.Errorf("all %d session slots are in use", MaxSessions)
}
