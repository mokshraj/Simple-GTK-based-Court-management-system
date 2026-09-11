// Package portal serves the GTK Broadway multi-tab portal.
//
// It exposes three HTTP endpoints:
//
//	GET  /portal/             → small HTML page that loads /portal/index.html
//	GET  /portal/index.html   → cleaned-up iframe shell
//	GET  /portal/launch?id=N  → spawn gtk4-broadwayd + gui for session N, redirect
//	POST /portal/notify?id=N  → open a URL in the host browser via xdg-open
//
// All child processes are spawned through the shared procman.Manager so the
// TUI can see and kill them.
package portal

import (
	"encoding/json"
	"fmt"
	"log"
	"net"
	"net/http"
	"net/http/httputil"
	"net/url"
	"os"
	"os/exec"
	"path/filepath"
	"strconv"
	"strings"
	"time"

	"court-server/internal/procman"

	"github.com/gorilla/websocket"
)

// BroadStart is the base port for gtk4-broadwayd daemons.
// The actual port for session N is BroadStart + N (matches the old behaviour).
const BroadStart = 8090

// Portal bundles the dependencies the HTTP handlers need.
type Portal struct {
	Sm          *SessionManager
	PM          *procman.Manager
	AppPath     string // absolute or relative path to the compiled gui binary
	PortalDir   string // absolute or relative path to the portal HTML root
	Reg         *SessionRegistry
	WS          *WSHub
	SessionOpen chan int // closed or receives session ID when a new session opens
}

// New creates a Portal with the given process manager, gui binary path, and
// portal HTML directory.
func New(sm *SessionManager, pm *procman.Manager, appPath, portalDir string) *Portal {
	return &Portal{Sm: sm, PM: pm, AppPath: appPath, PortalDir: portalDir, Reg: NewSessionRegistry(), WS: NewWSHub(), SessionOpen: make(chan int, 1)}
}

// RegisterRoutes wires the portal routes onto mux under the given prefix.
func (p *Portal) RegisterRoutes(mux *http.ServeMux) {
	// More specific routes first (Go's ServeMux uses longest match)
	mux.HandleFunc("/portal/embed", p.handleEmbed)
	mux.HandleFunc("/portal/ws", p.handleWebSocket)
	mux.HandleFunc("/portal/launch", p.handleLaunch)
	mux.HandleFunc("/portal/notify", p.handleNotify)
	mux.HandleFunc("/portal/index.html", p.handleIndexHTML)
	mux.HandleFunc("/api/portal/new", p.handleNewSession)
	mux.HandleFunc("/api/portal/heartbeat", p.handleHeartbeat)
	mux.HandleFunc("/api/portal/close", p.handleClose)
	mux.HandleFunc("/api/portal/sessions", p.handleListSessions)
	mux.HandleFunc("/portal/", p.handleIndex)

	// Reverse-proxy for Broadway sessions: /broadway/{id}/... -> http://127.0.0.1:8090+id/...
	// This keeps all traffic on the same HTTPS origin so browsers don't block
	// mixed-content HTTPS->HTTP redirects.
	mux.HandleFunc("/broadway/", p.handleBroadwayProxy)
}

// handleNewSession allocates a session id and returns its launch URL as JSON.
// The portal's static page calls this via fetch(); previously it 404'd and
// fell back to opening the wrong (un-allocated) session.
func (p *Portal) handleNewSession(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodPost {
		http.Error(w, "POST required", http.StatusMethodNotAllowed)
		return
	}
	id, err := p.Sm.GetNextID(p.Reg)
	if err != nil {
		http.Error(w, err.Error(), http.StatusServiceUnavailable)
		return
	}
	w.Header().Set("Content-Type", "application/json")
	w.Write([]byte(fmt.Sprintf(`{"id":%d,"url":"/portal/launch?id=%d"}`, id, id)))
}

// handleHeartbeat refreshes the last-heartbeat timestamp for a live session.
// The embed page calls this every 5s so the server can detect a closed tab.
func (p *Portal) handleHeartbeat(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodPost {
		http.Error(w, "POST required", http.StatusMethodNotAllowed)
		return
	}
	id, _ := strconv.Atoi(r.URL.Query().Get("id"))
	if id == 0 {
		http.Error(w, "missing id", http.StatusBadRequest)
		return
	}
	if err := p.Reg.Heartbeat(id); err != nil {
		http.Error(w, err.Error(), http.StatusNotFound)
		return
	}
	w.WriteHeader(http.StatusOK)
}

// handleClose terminates a session idempotently.
// Called when the user clicks the [✕] button or the embed tab is closing.
func (p *Portal) handleClose(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodPost {
		http.Error(w, "POST required", http.StatusMethodNotAllowed)
		return
	}
	id, _ := strconv.Atoi(r.URL.Query().Get("id"))
	if id == 0 {
		http.Error(w, "missing id", http.StatusBadRequest)
		return
	}
	p.Reg.Close(id)
	w.WriteHeader(http.StatusOK)
}

// handleListSessions returns a JSON array of all live sessions.
func (p *Portal) handleListSessions(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Content-Type", "application/json")
	sessions := p.Reg.List()
	if sessions == nil {
		sessions = []SessionEntry{}
	}
	fmt.Fprintf(w, "%s", mustMarshalJSON(sessions))
}

// handleEmbed serves the per-session iframe wrapper from the static file.
// The static embed.html contains:
//   - a heartbeat loop (POST /api/portal/heartbeat?id=N every 5s)
//   - a [✕ Close] button (POST /api/portal/close?id=N)
//   - an inner <iframe src="/broadway/N/"> that renders the GTK app
func (p *Portal) handleEmbed(w http.ResponseWriter, r *http.Request) {
	id, _ := strconv.Atoi(r.URL.Query().Get("id"))
	if id == 0 {
		http.Error(w, "missing id", http.StatusBadRequest)
		return
	}
	embedPath := filepath.Join(p.PortalDir, "embed.html")
	abs, err := filepath.Abs(embedPath)
	if err != nil {
		http.Error(w, "internal error", http.StatusInternalServerError)
		return
	}
	if _, err := os.Stat(abs); err != nil {
		http.Error(w, "embed template not found: "+abs, http.StatusNotFound)
		return
	}
	w.Header().Set("Content-Type", "text/html; charset=utf-8")
	http.ServeFile(w, r, abs)
}

// handleBroadwayProxy reverse-proxies /broadway/{id}/... to the matching
// gtk4-broadwayd daemon. id is parsed from the URL path; the broadway port is
// derived as BroadStart+id (must match the port handleLaunch started).
func (p *Portal) handleBroadwayProxy(w http.ResponseWriter, r *http.Request) {
	// Trim the prefix and split the first segment as the session id.
	rest := strings.TrimPrefix(r.URL.Path, "/broadway/")
	parts := strings.SplitN(rest, "/", 2)
	if len(parts) == 0 || parts[0] == "" {
		http.Error(w, "missing session id", http.StatusBadRequest)
		return
	}
	id, err := strconv.Atoi(parts[0])
	if err != nil || id < 1 || id > MaxSessions {
		http.Error(w, "invalid session id", http.StatusBadRequest)
		return
	}

	// Reconstruct the upstream URL pointing at the broadway daemon.
	broadwayPort := BroadStart + id
	upstream := &url.URL{
		Scheme: "http",
		Host:   fmt.Sprintf("127.0.0.1:%d", broadwayPort),
	}
	// Preserve any sub-path so broadway's WebSocket upgrade and other routes
	// work transparently.
	if len(parts) == 2 {
		upstream.Path = "/" + parts[1]
	} else {
		upstream.Path = "/"
	}
	upstream.RawQuery = r.URL.RawQuery

	rp := httputil.NewSingleHostReverseProxy(upstream)
	rp.Director = func(req *http.Request) {
		req.URL.Scheme = upstream.Scheme
		req.URL.Host = upstream.Host
		req.URL.Path = upstream.Path
		req.URL.RawQuery = upstream.RawQuery
		req.Host = upstream.Host // critical: broadwayd validates Host header
	}
	rp.ErrorHandler = func(rw http.ResponseWriter, req *http.Request, err error) {
		log.Printf("portal: broadway proxy error for session %d: %v", id, err)
		http.Error(rw, "broadway session unavailable", http.StatusBadGateway)
	}
	rp.ServeHTTP(w, r)
}

// handleIndex serves the iframe shell directly (kept inline for simplicity;
// the user may override via a static file at ./court-server/web/portal/index.html
// but we ship a built-in default so the binary is self-contained on first run).
func (p *Portal) handleIndex(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Cache-Control", "no-cache, no-store, must-revalidate")
	w.Header().Set("Pragma", "no-cache")
	w.Header().Set("Expires", "0")

	if r.URL.Path != "/portal/" && r.URL.Path != "/portal/index.html" {
		http.NotFound(w, r)
		return
	}

	// Hand off to the static handler if a custom file exists on disk.
	if abs, err := filepath.Abs(filepath.Join(p.PortalDir, "index.html")); err == nil {
		if _, err := os.Stat(abs); err == nil {
			http.ServeFile(w, r, abs)
			return
		}
	}

	// Built-in default — no id allocation here; let the JS allocate via
	// POST /api/portal/new so we don't burn session ids on page loads.
	w.Header().Set("Content-Type", "text/html; charset=utf-8")
	fmt.Fprint(w, `<!DOCTYPE html>
<html><head><title>Broadway Portal</title></head>
<body>
<button onclick="fetch('/api/portal/new',{method:'POST'})
    .then(r=>r.json()).then(d=>window.location='/portal/embed?id='+d.id)">
    New Session
</button>
</body></html>`)
}

func (p *Portal) handleIndexHTML(w http.ResponseWriter, r *http.Request) {
	p.handleIndex(w, r)
}

// handleLaunch is the core Broadway launcher.
func (p *Portal) handleLaunch(w http.ResponseWriter, r *http.Request) {
	idStr := r.URL.Query().Get("id")
	displayNum, err := strconv.Atoi(idStr)
	if err != nil || displayNum < 1 || displayNum > MaxSessions {
		http.Error(w, "invalid or missing id", http.StatusBadRequest)
		return
	}
	broadwayPort := BroadStart + displayNum
	displayStr := fmt.Sprintf(":%d", displayNum)

	absAppPath, _ := filepath.Abs(p.AppPath)

	// ── 1. Start gtk4-broadwayd ──────────────────────────────────────────
	bwCmd := exec.Command("gtk4-broadwayd",
		"--address", "::",
		"--port", fmt.Sprintf("%d", broadwayPort),
		displayStr,
	)
	if _, err := p.PM.StartAndRegister(fmt.Sprintf("broadway:%d", displayNum), "broadway", bwCmd, map[string]string{
		"display":       displayStr,
		"broadway_port": strconv.Itoa(broadwayPort),
	}); err != nil {
		log.Printf("portal: broadway start: %v", err)
		http.Error(w, "failed to start broadway daemon", http.StatusInternalServerError)
		return
	}

	// Wait for the broadway port to be ready (up to 3s).
	ready := false
	for i := 0; i < 30; i++ {
		conn, err := net.DialTimeout("tcp",
			fmt.Sprintf("127.0.0.1:%d", broadwayPort), 50*time.Millisecond)
		if err == nil {
			conn.Close()
			ready = true
			break
		}
		time.Sleep(100 * time.Millisecond)
	}
	if !ready {
		log.Printf("portal: broadway port %d not ready", broadwayPort)
	}

	// ── 2. Launch the GTK client ─────────────────────────────────────────
	appCmd := exec.Command(absAppPath, "web")
	appCmd.Dir = filepath.Dir(absAppPath)
	appCmd.Env = []string{
		"GDK_BACKEND=broadway",
		"BROADWAY_DISPLAY=" + displayStr,
		"DBUS_SESSION_BUS_ADDRESS=unix:path=/dev/null",
		"XDG_RUNTIME_DIR=" + os.Getenv("XDG_RUNTIME_DIR"),
		"PATH=" + os.Getenv("PATH"),
		"SESSION_ID=" + idStr,
	}
	guiID := fmt.Sprintf("gui:%d", displayNum)
	if _, err := p.PM.StartAndRegister(guiID, "gui", appCmd, map[string]string{
		"display": displayStr,
		"app":     absAppPath,
	}); err != nil {
		log.Printf("portal: gui register: %v", err)
		_ = killCmd(bwCmd)
		http.Error(w, "failed to start gui: "+err.Error(), http.StatusInternalServerError)
		return
	}

	// Register the session in the lifecycle registry and start the watcher.
	// The watcher will clean up both processes when the gui exits, the
	// heartbeat times out, or the tab sends a close request.
	state, err := p.Reg.Register(displayNum, bwCmd, appCmd, broadwayPort)
	if err != nil {
		log.Printf("portal: register session %d: %v", displayNum, err)
		_ = killCmd(bwCmd)
		_ = killCmd(appCmd)
		http.Error(w, "session already active", http.StatusConflict)
		return
	}
	go p.Reg.Watch(state)

	// Notify listeners (e.g. TUI) that a new session has opened.
	select {
	case p.SessionOpen <- displayNum:
	default:
	}

	// ── 3. Return JSON response with the broadway URL ─────────────────────
	// Return JSON instead of redirect so the client can handle it properly
	// (especially when called from an iframe).
	w.Header().Set("Content-Type", "application/json")
	fmt.Fprintf(w, `{"success":true,"broadway_url":"/broadway/%d/"}`, displayNum)
}

// handleNotify opens a URL in the host browser (called by the GTK app).
// For remote usage, it sends a WebSocket message to the browser client.
// Falls back to xdg-open if no WebSocket client is connected.
func (p *Portal) handleNotify(w http.ResponseWriter, r *http.Request) {
	id, _ := strconv.Atoi(r.URL.Query().Get("id"))
	url := r.URL.Query().Get("url")
	if url == "" {
		http.Error(w, "missing url", http.StatusBadRequest)
		return
	}
	log.Printf("portal: session %d requested to open %s", id, url)

	// Try to send via WebSocket to the browser client.
	if p.WS != nil {
		p.WS.BroadcastToSession(id, wsMessage{Type: "open_url", URL: url})
	}

	// Fallback: try xdg-open for local development.
	go func() {
		cmd := exec.Command("xdg-open", url)
		cmd.Stdout = os.Stdout
		cmd.Stderr = os.Stderr
		if err := cmd.Run(); err != nil {
			log.Printf("portal: xdg-open fallback: %v", err)
		}
	}()

	w.WriteHeader(http.StatusOK)
	w.Write([]byte("OK"))
}

// handleWebSocket upgrades the HTTP connection to a WebSocket and registers
// it with the hub for the given session. The session ID is passed as a
// query parameter: /portal/ws?session=<id>
func (p *Portal) handleWebSocket(w http.ResponseWriter, r *http.Request) {
	sessionID, _ := strconv.Atoi(r.URL.Query().Get("session"))
	if sessionID == 0 {
		http.Error(w, "missing session", http.StatusBadRequest)
		return
	}

	upgrader := websocket.Upgrader{
		CheckOrigin: func(r *http.Request) bool { return true },
	}
	conn, err := upgrader.Upgrade(w, r, nil)
	if err != nil {
		log.Printf("portal: ws upgrade error session=%d: %v", sessionID, err)
		return
	}

	p.WS.Register(sessionID, conn)
	defer p.WS.Unregister(sessionID, conn)

	// Keep the connection open until the client disconnects.
	for {
		if _, _, err := conn.ReadMessage(); err != nil {
			break
		}
	}
}

// mustMarshalJSON is a tiny helper that panics on marshal failure.
func mustMarshalJSON(v interface{}) []byte {
	b, err := json.Marshal(v)
	if err != nil {
		panic(err)
	}
	return b
}
