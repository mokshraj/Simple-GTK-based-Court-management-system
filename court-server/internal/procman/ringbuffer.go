package procman

import "sync"

// RingBuffer is a fixed-capacity log buffer safe for concurrent use.
type RingBuffer struct {
	mu   sync.Mutex
	data []string
	cap  int
	idx  int
	full bool
}

func NewRingBuffer(capacity int) *RingBuffer {
	if capacity <= 0 {
		capacity = 100
	}
	return &RingBuffer{data: make([]string, capacity), cap: capacity}
}

func (r *RingBuffer) Push(line string) {
	r.mu.Lock()
	defer r.mu.Unlock()
	r.data[r.idx] = line
	r.idx = (r.idx + 1) % r.cap
	if r.idx == 0 {
		r.full = true
	}
}

// Tail returns the last n lines in chronological order.
// n <= 0 returns all lines.
func (r *RingBuffer) Tail(n int) []string {
	r.mu.Lock()
	defer r.mu.Unlock()
	var lines []string
	if r.full {
		lines = append(lines, r.data[r.idx:]...)
		lines = append(lines, r.data[:r.idx]...)
	} else {
		lines = append(lines, r.data[:r.idx]...)
	}
	if n > 0 && n < len(lines) {
		return lines[len(lines)-n:]
	}
	return lines
}

// Len returns the number of lines currently stored.
func (r *RingBuffer) Len() int {
	r.mu.Lock()
	defer r.mu.Unlock()
	if r.full {
		return r.cap
	}
	return r.idx
}
