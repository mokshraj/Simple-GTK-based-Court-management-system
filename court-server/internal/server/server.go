// Package server builds and runs the unified HTTP/HTTPS server for
// court-server. It wires together the static filesystem, the meet
// WebSocket + REST handler, and the Broadway portal.
package server

import (
	"context"
	"log"
	"net"
	"net/http"
	"time"

	"court-server/internal/config"
	"court-server/internal/meet/api"
	"court-server/internal/meet/room"
	"court-server/internal/portal"
	"court-server/internal/procman"
	"court-server/internal/staticfs"
)

// Server bundles the running http.Server and its dependencies so the TUI
// can stop it cleanly on shutdown.
type Server struct {
	Cfg    *config.Config
	HTTP   *http.Server
	Portal *portal.Portal
	PM     *procman.Manager
	RM     *room.Manager
}

// New constructs the Server with all routes registered but does not listen yet.
func New(cfg *config.Config, pm *procman.Manager) (*Server, error) {
	sm := portal.NewSessionManager()
	p := portal.New(sm, pm, cfg.GUIAppPath, cfg.PortalDir)
	rm := room.NewManager()
	mh := api.NewHandler(rm)

	// Set up WebSocket session close callback to clean up processes
	p.WS.OnSessionClose = func(sessionID int) {
		log.Printf("portal: WebSocket closed for session %d, cleaning up", sessionID)
		p.Reg.Close(sessionID)
	}

	mux := http.NewServeMux()

	// Static
	mux.Handle("/site/", staticfs.NoCache(staticfs.Dir("/site/", cfg.SiteDir)))
	mux.HandleFunc("/site/Judgment.db", staticfs.JudgmentDB())
	mux.Handle("/meet/", staticfs.NoCache(staticfs.Dir("/meet/", cfg.MeetDir)))

	// Meet
	mux.HandleFunc("/ws", mh.HandleWS)
	mux.HandleFunc("/rooms", mh.HandleRooms)

	// Portal
	p.RegisterRoutes(mux)

	// Root: redirect to the site so https://host:1753/ works.
	mux.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		if r.URL.Path != "/" {
			http.NotFound(w, r)
			return
		}
		http.Redirect(w, r, "/site/", http.StatusTemporaryRedirect)
	})

	// API (TUI)
	mux.HandleFunc("/api/status", func(w http.ResponseWriter, r *http.Request) {
		w.Header().Set("Content-Type", "application/json")
		// simple JSON without a real encoder dep
		w.Write([]byte(`{"procs":`))
		writeProcs(w, pm.Snapshot())
		w.Write([]byte(`}`))
	})

	srv := &http.Server{
		Addr:              cfg.Addr,
		Handler:           mux,
		ReadHeaderTimeout: 10 * time.Second,
	}

	return &Server{
		Cfg:    cfg,
		HTTP:   srv,
		Portal: p,
		PM:     pm,
		RM:     rm,
	}, nil
}

// Run starts the HTTPS listener; it blocks until ctx is cancelled or the
// server crashes. It auto-generates a self-signed cert if none is found.
func (s *Server) Run(ctx context.Context) error {
	cert, key, err := resolveCert(s.Cfg)
	if err != nil {
		return err
	}
	s.HTTP.TLSConfig = nil
	log.Printf("server: listening on %s (TLS cert=%s)", s.Cfg.Addr, cert)
	go func() {
		<-ctx.Done()
		shutdownCtx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
		defer cancel()
		_ = s.HTTP.Shutdown(shutdownCtx)
	}()
	if err := s.HTTP.ListenAndServeTLS(cert, key); err != nil && err != http.ErrServerClosed {
		return err
	}
	return nil
}

// Stop performs an orderly shutdown: kills all child processes, then closes
// the HTTP server.
func (s *Server) Stop() {
	log.Printf("server: shutting down")
	s.PM.Shutdown(2 * time.Second)
	ctx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
	defer cancel()
	_ = s.HTTP.Shutdown(ctx)
}

// Listener tries to bind the address to find a free port (for the
// auto-detect BaseURL flow). Useful at startup.
func Listener(addr string) (net.Listener, error) {
	return net.Listen("tcp", addr)
}
