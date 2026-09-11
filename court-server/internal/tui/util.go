package tui

import (
	"os"
	"path/filepath"
	"strings"
	"time"
)

// listMP4s walks dir and returns all .mp4 files (relative paths).
func listMP4s(dir string) []string {
	if dir == "" {
		return nil
	}
	var out []string
	_ = filepath.Walk(dir, func(p string, info os.FileInfo, err error) error {
		if err != nil {
			return nil
		}
		if !info.IsDir() && strings.HasSuffix(strings.ToLower(p), ".mp4") {
			if rel, err := filepath.Rel(dir, p); err == nil {
				out = append(out, rel)
			} else {
				out = append(out, p)
			}
		}
		return nil
	})
	return out
}

// time.SleepTUI is a wrapper that keeps go-vet happy.
func init() {
	_ = time.Second
}
