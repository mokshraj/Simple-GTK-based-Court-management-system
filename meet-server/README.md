# Meet Server

A self-hosted Google-Meet-like system in Go with:
- WebRTC peer-to-peer video/audio (browser ↔ browser)
- WebSocket signaling relay
- GStreamer-based meeting recording (per-room WebM files)
- **role=client** — connect your own application to control meetings

---

## Architecture

```
┌─────────────────────────────────────────────────────────────────────┐
│                        meet-server  :1755                            │
│                                                                     │
│  /ws?role=client  ◀──── Your App ─────▶  open_meeting               │
│                          (Go/Python/etc)   close_meeting             │
│                                            ← meeting_link response  │
│                                                                     │
│  /ws?role=browser&room=<id>                                         │
│         ◀── Browser A ──▶  WebRTC signaling relay                   │
│         ◀── Browser B ──▶  (offer/answer/ICE candidate)             │
│                                                                     │
│  GStreamer pipeline (one per room)                                  │
│         UDP:5000 (Opus audio RTP) ──┐                               │
│         UDP:5002 (VP8  video RTP) ──┴──▶ recordings/<room>/*.webm  │
└─────────────────────────────────────────────────────────────────────┘
```

---

## Prerequisites

### Go
```
go 1.21+
```

### GStreamer (for recording)
**Ubuntu/Debian:**
```bash
sudo apt install \
  gstreamer1.0-tools \
  gstreamer1.0-plugins-base \
  gstreamer1.0-plugins-good \
  gstreamer1.0-plugins-bad \
  gstreamer1.0-libav
```

**macOS:**
```bash
brew install gstreamer gst-plugins-base gst-plugins-good gst-plugins-bad gst-libav
```

> The server works without GStreamer installed — recording just won't start. It will log the UDP ports it would have used.

---

## Setup & Run

```bash
# 1. Clone / place files
cd meet-server

# 2. Download dependencies
go mod tidy

# 3. Run the server
go run main.go

# Optional: custom recordings folder
go run main.go /path/to/my/recordings
```

The server starts at **http://localhost:1755**

---

## Roles & WebSocket Protocol

### role=client  (`ws://localhost:1755/ws?role=client`)

Your backend application connects here to control meetings.

**Send → Server:**
```json
{ "type": "open_meeting" }
```
```json
{ "type": "close_meeting", "payload": { "room_id": "uuid" } }
```

**Receive ← Server:**
```json
{
  "type": "meeting_link",
  "payload": {
    "room_id": "550e8400-e29b-41d4-a716-446655440000",
    "url":     "http://localhost:1755/?room=550e8400-e29b-41d4-a716-446655440000"
  }
}
```

### role=browser  (`ws://localhost:1755/ws?role=browser&room=<id>`)

The browser UI connects here automatically when the user opens the meeting URL.  
The server relays WebRTC signaling between peers in the same room.

**Signaling messages (browser ↔ server ↔ browser):**
```json
{ "type": "offer",     "payload": { "from": "peer-A", "to": "peer-B", "sdp": "..." } }
{ "type": "answer",    "payload": { "from": "peer-B", "to": "peer-A", "sdp": "..." } }
{ "type": "candidate", "payload": { "from": "peer-A", "to": "peer-B", "candidate": {...} } }
```

**Server → browser notifications:**
```json
{ "type": "welcome",     "payload": { "peer_id": "...", "room_id": "...", "peers": ["..."] } }
{ "type": "peer_joined", "payload": { "peer_id": "..." } }
{ "type": "peer_left",   "payload": { "peer_id": "..." } }
{ "type": "room_closed" }
```

---

## GStreamer Recording

When a room is created, the recorder:
1. Calculates two UDP ports for that room (deterministic hash of room ID)
2. Launches a `gst-launch-1.0` pipeline listening on those ports
3. Muxes audio + video into `recordings/<room_id>/<timestamp>.webm`
4. Stops and flushes the file when the last peer leaves or the room is closed

**To feed RTP into the recorder**, your WebRTC layer (e.g. [pion/webrtc](https://github.com/pion/webrtc))
must forward decoded RTP packets to the UDP ports logged at startup:

```
recorder: started for room abc123  audio=5012  video=5014  → recordings/abc123/2024-01-15_10-30-00.webm
```

For a complete pion/webrtc integration, see the [pion examples](https://github.com/pion/webrtc/tree/master/examples).

---

## REST API

| Endpoint  | Method | Description              |
|-----------|--------|--------------------------|
| `/rooms`  | GET    | List active room IDs     |
| `/ws`     | GET    | WebSocket upgrade        |
| `/`       | GET    | Browser meeting UI       |

---

## Example Client App

```bash
go run example_client/main.go
```

```
✅ Connected to meet-server as [client]
Commands:  open  |  close <room_id>  |  quit
──────────────────────────────────────────────────
> open

🔗 Meeting created!
   Room ID : 550e8400-e29b-41d4-a716-446655440000
   URL     : http://localhost:1755/?room=550e8400-e29b-41d4-a716-446655440000

> close 550e8400-e29b-41d4-a716-446655440000

🔴 Room 550e8400-e29b-41d4-a716-446655440000 closed
```

---

## File Structure

```
meet-server/
├── main.go                    # Entry point
├── go.mod / go.sum
├── static/
│   └── index.html             # Browser meeting UI (WebRTC)
├── recordings/                # Auto-created; one folder per room
│   └── <room-id>/
│       └── 2024-01-15_10-30.webm
├── internal/
│   ├── room/
│   │   ├── room.go            # Room manager, peer registry, message types
│   │   └── recorder.go        # GStreamer pipeline per room
│   └── api/
│       └── handler.go         # WebSocket upgrade, role dispatch, signaling relay
└── example_client/
    └── main.go                # Demo client app (role=client)
```
