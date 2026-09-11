// Package staticfs provides disk-backed HTTP handlers for serving the
// website, the meet browser UI, and the portal landing page.
//
// All paths are resolved relative to the current working directory; pass
// absolute paths if the binary is run from elsewhere.
package staticfs

import (
	"net/http"
	"os"
	"path/filepath"
	"time"
)

// Dir returns an http.Handler that serves files from `root` under URL prefix
// `prefix`. The root is resolved to an absolute path so it works regardless
// of the process's CWD at request time.
func Dir(prefix, root string) http.Handler {
	if abs, err := filepath.Abs(root); err == nil {
		root = abs
	}
	fs := http.FileServer(http.Dir(root))
	return http.StripPrefix(prefix, fs)
}

// NoCache wraps an http.Handler with cache-busting headers for HTML.
func NoCache(h http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		w.Header().Set("Cache-Control", "no-cache, no-store, must-revalidate")
		w.Header().Set("Pragma", "no-cache")
		w.Header().Set("Expires", "0")
		h.ServeHTTP(w, r)
	})
}

// JudgmentDB serves ./Judgment.db from disk. Used when the DB is not co-located
// with the website/ directory.
func JudgmentDB() http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		f, err := os.Open("Judgment.db")
		if err != nil {
			http.Error(w, err.Error(), http.StatusNotFound)
			return
		}
		defer f.Close()
		w.Header().Set("Content-Type", "application/x-sqlite3")
		http.ServeContent(w, r, "Judgment.db", time.Now(), f)
	}
}
