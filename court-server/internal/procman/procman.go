// Package procman is a shared registry of all child processes spawned by the
// court-server (gtk4-broadwayd, gui, gst-launch-1.0, xdg-open, etc.).
//
// It is the single source of truth for:
//
//   - what's currently running
//   - stdout/stderr capture (tailed by the TUI Logs tab)
//   - graceful shutdown (children first, then parents, with a 2s SIGKILL grace)
package procman

import (
	"bufio"
	"fmt"
	"io"
	"log"
	"os/exec"
	"sync"
	"time"
)

// Event is broadcast to TUI subscribers when a process starts, exits, or is
// killed.
type Event struct {
	ID     string
	Kind   string
	Op     string // "spawn" | "exit" | "kill"
	Err    error
	Stderr string // one-line tail if exit was non-zero
}

// Proc is one managed child process.
type Proc struct {
	ID        string
	Kind      string
	Cmd       *exec.Cmd
	PID       int
	StartedAt time.Time
	Meta      map[string]string

	mu   sync.Mutex
	done chan struct{}
}

// Manager is the global process registry.
type Manager struct {
	mu      sync.RWMutex
	procs   map[string]*Proc
	buffers map[string]*RingBuffer // per-process log buffer (last N lines)
	events  chan Event

	bufferLines int // ring buffer capacity
}

// NewManager creates a Manager with sensible defaults.
func NewManager() *Manager {
	return &Manager{
		procs:       make(map[string]*Proc),
		buffers:     make(map[string]*RingBuffer),
		events:      make(chan Event, 128),
		bufferLines: 500,
	}
}

// Subscribe returns a channel of events and an unsubscribe function.
func (m *Manager) Subscribe() (<-chan Event, func()) {
	return m.events, func() {}
}

// StartAndRegister wires stdout/stderr pipes, starts the command, and registers
// it in the manager in one atomic step. This is the preferred way to add a
// process because it guarantees log capture (pipes must be created before
// cmd.Start()).
func (m *Manager) StartAndRegister(id, kind string, cmd *exec.Cmd, meta map[string]string) (*Proc, error) {
	buf := NewRingBuffer(m.bufferLines)

	// Wire pipes BEFORE Start so we actually capture output.
	stdout, err := cmd.StdoutPipe()
	if err != nil {
		return nil, fmt.Errorf("stdout pipe: %w", err)
	}
	stderr, err := cmd.StderrPipe()
	if err != nil {
		return nil, fmt.Errorf("stderr pipe: %w", err)
	}

	if err := cmd.Start(); err != nil {
		return nil, fmt.Errorf("start: %w", err)
	}

	go pump(buf, stdout, id, "stdout")
	go pump(buf, stderr, id, "stderr")

	p := &Proc{
		ID:        id,
		Kind:      kind,
		Cmd:       cmd,
		PID:       cmd.Process.Pid,
		StartedAt: time.Now(),
		Meta:      meta,
		done:      make(chan struct{}),
	}

	m.mu.Lock()
	m.procs[id] = p
	m.buffers[id] = buf
	m.mu.Unlock()

	m.broadcast(Event{ID: id, Kind: kind, Op: "spawn"})

	go func() {
		err := cmd.Wait()
		close(p.done)
		m.mu.Lock()
		delete(m.procs, id)
		m.mu.Unlock()

		ev := Event{ID: id, Kind: kind, Op: "exit", Err: err}
		if err != nil {
			tail := buf.Tail(1)
			if len(tail) > 0 {
				ev.Stderr = tail[0]
			}
		}
		m.broadcast(ev)
	}()

	return p, nil
}

// Register adds a process to the registry. The caller must have already called
// cmd.Start(). Register sets up a goroutine that waits for exit, broadcasts an
// event, and captures stdout+stderr to a ring buffer.
//
// Deprecated: Use StartAndRegister for new code so that stdout/stderr pipes are
// created before cmd.Start() and log capture actually works.
func (m *Manager) Register(id, kind string, cmd *exec.Cmd, meta map[string]string) (*Proc, error) {
	if cmd == nil || cmd.Process == nil {
		return nil, fmt.Errorf("cmd not started")
	}
	p := &Proc{
		ID:        id,
		Kind:      kind,
		Cmd:       cmd,
		PID:       cmd.Process.Pid,
		StartedAt: time.Now(),
		Meta:      meta,
		done:      make(chan struct{}),
	}
	buf := NewRingBuffer(m.bufferLines)

	// Wire pipes.
	stdout, _ := cmd.StdoutPipe()
	stderr, _ := cmd.StderrPipe()
	go pump(buf, stdout, id, "stdout")
	go pump(buf, stderr, id, "stderr")

	m.mu.Lock()
	m.procs[id] = p
	m.buffers[id] = buf
	m.mu.Unlock()

	m.broadcast(Event{ID: id, Kind: kind, Op: "spawn"})

	go func() {
		err := cmd.Wait()
		close(p.done)
		m.mu.Lock()
		delete(m.procs, id)
		m.mu.Unlock()

		ev := Event{ID: id, Kind: kind, Op: "exit", Err: err}
		if err != nil {
			tail := buf.Tail(1)
			if len(tail) > 0 {
				ev.Stderr = tail[0]
			}
		}
		m.broadcast(ev)
	}()

	return p, nil
}

// Kill sends SIGINT and, if still alive after grace, SIGKILL.
func (m *Manager) Kill(id string, grace time.Duration) error {
	m.mu.RLock()
	p, ok := m.procs[id]
	m.mu.RUnlock()
	if !ok {
		return fmt.Errorf("no such process: %s", id)
	}
	if err := p.Cmd.Process.Signal(interrupt); err != nil {
		return p.Cmd.Process.Kill()
	}
	select {
	case <-p.done:
		// graceful exit
	case <-time.After(grace):
		_ = p.Cmd.Process.Kill()
		<-p.done
	}
	m.broadcast(Event{ID: id, Kind: p.Kind, Op: "kill"})
	return nil
}

// Shutdown kills every process in reverse-spawn order.
func (m *Manager) Shutdown(grace time.Duration) {
	m.mu.RLock()
	ids := make([]string, 0, len(m.procs))
	for id := range m.procs {
		ids = append(ids, id)
	}
	m.mu.RUnlock()
	// reverse for child-before-parent
	for i := len(ids) - 1; i >= 0; i-- {
		_ = m.Kill(ids[i], grace)
	}
}

// Snapshot returns a JSON-serialisable view of all live processes.
type Snapshot struct {
	ID        string            `json:"id"`
	Kind      string            `json:"kind"`
	PID       int               `json:"pid"`
	StartedAt time.Time         `json:"started_at"`
	Uptime    string            `json:"uptime"`
	Meta      map[string]string `json:"meta,omitempty"`
}

func (m *Manager) Snapshot() []Snapshot {
	m.mu.RLock()
	defer m.mu.RUnlock()
	out := make([]Snapshot, 0, len(m.procs))
	for _, p := range m.procs {
		out = append(out, Snapshot{
			ID:        p.ID,
			Kind:      p.Kind,
			PID:       p.PID,
			StartedAt: p.StartedAt,
			Uptime:    time.Since(p.StartedAt).Round(time.Second).String(),
			Meta:      p.Meta,
		})
	}
	return out
}

// Tail returns the last n log lines for a given process id (empty slice if unknown).
func (m *Manager) Tail(id string, n int) []string {
	m.mu.RLock()
	buf, ok := m.buffers[id]
	m.mu.RUnlock()
	if !ok {
		return nil
	}
	return buf.Tail(n)
}

// AllBuffers returns every process id and its ring buffer (for the Logs tab).
func (m *Manager) AllBuffers() map[string]*RingBuffer {
	m.mu.RLock()
	defer m.mu.RUnlock()
	out := make(map[string]*RingBuffer, len(m.buffers))
	for k, v := range m.buffers {
		out[k] = v
	}
	return out
}

func (m *Manager) broadcast(ev Event) {
	select {
	case m.events <- ev:
	default:
		// drop if subscriber is slow; log to stdout
		log.Printf("procman: event buffer full, dropping %s/%s", ev.Op, ev.ID)
	}
}

func pump(buf *RingBuffer, r io.ReadCloser, id, stream string) {
	if r == nil {
		return
	}
	defer r.Close()
	sc := bufio.NewScanner(r)
	sc.Buffer(make([]byte, 64*1024), 1024*1024)
	for sc.Scan() {
		buf.Push(fmt.Sprintf("[%s:%s] %s", id, stream, sc.Text()))
	}
	if err := sc.Err(); err != nil {
		buf.Push(fmt.Sprintf("[%s:%s] scanner error: %v", id, stream, err))
	}
}
