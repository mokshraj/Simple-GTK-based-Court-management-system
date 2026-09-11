# Court Management System — PBL Semester 3

A self-hosted Indian Judiciary platform combining:

- **Static case / lawyer / judge browser** (HTML + sql.js, no backend)
- **GTK desktop client** ([`gui.c`](gui.c:1), rendered remotely via Broadway)
- **WebRTC video meetings** (browser peer-to-peer, GStreamer recording)
- **Unified Go server** ([`court-server/`](court-server/)) replacing the
  old two-binary setup with one process plus a TUI control panel

---

## Quick start

The unified server replaces the previous dual-binary setup. From the workspace
root:

```bash
./court-server/run.sh
```

That builds (if needed) and launches `court-server` with the interactive TUI.
The first run auto-generates a self-signed TLS cert at
[`court-server/certs/cert.pem`](court-server/certs/cert.pem).

### Open in your browser

| URL | What you'll see |
|---|---|
| `https://localhost:1753/site/` | Static case / lawyer / judge browser |
| `https://localhost:1753/portal/` | GTK Broadway multi-tab portal |
| `https://localhost:1753/meet/?room=<id>` | WebRTC video meeting |
| `https://localhost:1753/api/status` | JSON snapshot of running processes |

> **Self-signed cert:** the browser will warn once. Click "Advanced → Proceed
> to localhost (unsafe)" to allow the secure context required for WebRTC
> `getUserMedia`.

### Headless / CI

```bash
./court-server/run.sh --no-tui
```

### TUI keys

| Key | Action |
|---|---|
| `1`..`6` | jump to tab |
| `Tab` / `Shift+Tab` | cycle tabs |
| `r` | refresh current tab |
| `q` | quit (graceful shutdown of all child processes) |

Tabs: Dashboard · Broadway · Meet · Recordings · Logs · Settings

---

## Repository layout

```
.
├── court-server/                 ← NEW unified Go binary
│   ├── cmd/court-server/         ← entry point (main.go)
│   ├── internal/
│   │   ├── server/               ← TLS mux, route table
│   │   ├── portal/               ← Broadway launch + xdg-open notify
│   │   ├── meet/                 ← WebSocket + room manager + recorder
│   │   ├── staticfs/             ← disk-backed FileServer helpers
│   │   ├── tlsutil/              ← auto-generate self-signed cert
│   │   ├── procman/              ← child-process registry (shared with TUI)
│   │   ├── tui/                  ← rivo/tview control panel
│   │   └── config/               ← CLI flags + defaults
│   ├── certs/                    ← TLS certs (auto-gen on first run)
│   ├── recordings/               ← GStreamer meet recordings
│   └── web/portal/index.html     ← cleaned portal landing page
├── website/                      ← static site (served at /site/*)
├── meet-server/static/           ← meet browser UI (served at /meet/*)
├── Judgment.db                   ← SQLite DB (served at /site/Judgment.db)
├── gui.c / gui.h / gui           ← GTK desktop client
├── Go_backend/                   ← (deprecated stub) old Broadway server
└── meet-server/                  ← (deprecated stub) old TLS meet server
```

---

## Architecture

The unified server listens on **a single TLS port** ([`:1753`](court-server/internal/server/server.go))
with a single mux:

```
Browser ─HTTPS:1753─► Mux
   ├── /site/*       → http.FileServer(./website)        (live from disk)
   ├── /site/Judgment.db → ./Judgment.db
   ├── /meet/*       → http.FileServer(./meet-server/static)
   ├── /ws, /rooms   → meet WebSocket + REST
   ├── /portal/*     → Broadway launch + GTK iframe shell
   └── /api/*        → status JSON for the TUI
```

The TLS cert is auto-generated on first run with every non-loopback local
IP in the SAN list, so the browser accepts it for any address the user
browses from. The cert is persisted to
[`court-server/certs/cert.pem`](court-server/certs/cert.pem) for reuse.

Static files are served **straight from disk** — edit any HTML/CSS/JS in
`./website/` or `./meet-server/static/` and refresh the browser. No
rebuild required.

The **child-process manager** ([`procman`](court-server/internal/procman/procman.go))
owns every `gtk4-broadwayd`, `gui`, `gst-launch-1.0`, and `xdg-open` process.
The TUI's Logs tab tails their captured stdout/stderr; the Broadway tab
lists the live GTK sessions; the Recordings tab walks the GStreamer output
directory.

---

## Building from source

```bash
cd court-server
make build           # produces ./court-server
./court-server       # or: make run
```

Requirements: Go 1.24+, GTK 4 (libgtk-4-dev), gtk4-broadwayd, GStreamer
1.x with the good/bad/libav plugin sets.

---

## Old / deprecated

The previous dual-server setup is preserved as thin stub binaries in
[`Go_backend/main.go`](Go_backend/main.go:1) and
[`meet-server/main.go`](meet-server/main.go:1). They print a deprecation
message and exit. Use [`court-server/cmd/court-server`](court-server/cmd/court-server/main.go)
instead.

The Python chatbot that previously lived at the repo root
(`newcahtbot.py`, `context.py`) has been removed as it was unrelated to
the unified-server scope.
