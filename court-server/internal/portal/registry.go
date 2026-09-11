package portal

import (
	"fmt"
	"os"
	"os/exec"
	"sync"
	"time"
)

// SessionEntry is the JSON-serializable view of a live session.
type SessionEntry struct {
	ID            int       `json:"id"`
	GuiPID        int       `json:"guiPid"`
	BroadwayPID   int       `json:"broadwayPid"`
	BroadwayPort  int       `json:"broadwayPort"`
	StartedAt     time.Time `json:"startedAt"`
	LastHeartbeat time.Time `json:"lastHeartbeat"`
}

type sessionState struct {
	entry     SessionEntry
	bwCmd     *exec.Cmd
	appCmd    *exec.Cmd
	quit      chan struct{}
	closeOnce sync.Once
}

// SessionRegistry tracks live Broadway sessions and owns their lifecycle.
type SessionRegistry struct {
	mu       sync.RWMutex
	sessions map[int]*sessionState
}

func NewSessionRegistry() *SessionRegistry {
	return &SessionRegistry{sessions: make(map[int]*sessionState)}
}

// Register adds a new session. It returns an error if the id is already taken.
func (r *SessionRegistry) Register(id int, bwCmd, appCmd *exec.Cmd, broadwayPort int) (*sessionState, error) {
	r.mu.Lock()
	defer r.mu.Unlock()
	if _, exists := r.sessions[id]; exists {
		return nil, fmt.Errorf("session %d already active", id)
	}
	state := &sessionState{
		entry: SessionEntry{
			ID:            id,
			GuiPID:        appCmd.Process.Pid,
			BroadwayPID:   bwCmd.Process.Pid,
			BroadwayPort:  broadwayPort,
			StartedAt:     time.Now(),
			LastHeartbeat: time.Now(),
		},
		bwCmd:  bwCmd,
		appCmd: appCmd,
		quit:   make(chan struct{}),
	}
	r.sessions[id] = state
	return state, nil
}

// Heartbeat refreshes the last-heartbeat timestamp for session id.
func (r *SessionRegistry) Heartbeat(id int) error {
	r.mu.Lock()
	defer r.mu.Unlock()
	s, ok := r.sessions[id]
	if !ok {
		return fmt.Errorf("session %d not found", id)
	}
	s.entry.LastHeartbeat = time.Now()
	return nil
}

// Close terminates a session idempotently.
func (r *SessionRegistry) Close(id int) {
	r.mu.Lock()
	s, ok := r.sessions[id]
	if !ok {
		r.mu.Unlock()
		return
	}
	r.mu.Unlock()

	s.closeOnce.Do(func() {
		close(s.quit)
		// Best-effort kill of both processes.
		_ = killCmd(s.appCmd)
		_ = killCmd(s.bwCmd)
	})

	r.mu.Lock()
	delete(r.sessions, id)
	r.mu.Unlock()
}

// List returns a snapshot of all live sessions.
func (r *SessionRegistry) List() []SessionEntry {
	r.mu.RLock()
	defer r.mu.RUnlock()
	out := make([]SessionEntry, 0, len(r.sessions))
	for _, s := range r.sessions {
		out = append(out, s.entry)
	}
	return out
}

// Get returns the session state for id, if it exists.
func (r *SessionRegistry) Get(id int) (*sessionState, bool) {
	r.mu.RLock()
	defer r.mu.RUnlock()
	s, ok := r.sessions[id]
	return s, ok
}

// Watch runs the lifecycle goroutine for one session.
// It returns when the session has been fully torn down.
func (r *SessionRegistry) Watch(state *sessionState) {
	id := state.entry.ID
	go func() {
		trigger := make(chan struct{}, 1)
		go func() {
			<-state.quit
			trigger <- struct{}{}
		}()
		go func() {
			err := state.appCmd.Wait()
			if err != nil {
				// gui exited with an error — still trigger cleanup
				_ = err
			}
			trigger <- struct{}{}
		}()

		ticker := time.NewTicker(5 * time.Second)
		defer ticker.Stop()

		for {
			select {
			case <-trigger:
				goto cleanup
			case <-ticker.C:
				r.mu.RLock()
				s, ok := r.sessions[id]
				r.mu.RUnlock()
				if !ok {
					goto cleanup
				}
				if time.Since(s.entry.LastHeartbeat) > 15*time.Second {
					goto cleanup
				}
			}
		}

	cleanup:
		r.Close(id)
	}()
}

// killCmd sends SIGINT then SIGKILL after a short grace.
func killCmd(cmd *exec.Cmd) error {
	if cmd == nil || cmd.Process == nil {
		return nil
	}
	if err := cmd.Process.Signal(os.Interrupt); err != nil {
		_ = cmd.Process.Kill()
		return err
	}
	done := make(chan struct{})
	go func() {
		cmd.Process.Wait()
		close(done)
	}()
	select {
	case <-done:
		return nil
	case <-time.After(2 * time.Second):
		_ = cmd.Process.Kill()
		<-done
		return fmt.Errorf("process killed after timeout")
	}
}
