package room

import (
	"fmt"
	"log"
	"os"
	"os/exec"
	"path/filepath"
	"strings"
	"time"
)

// Recorder manages a GStreamer pipeline that records audio+video for a room.
//
// Container : MP4  (H.264 video + AAC audio)
//
// Pipeline overview:
//   mp4mux name=muxer fragment-duration=500 faststart=true ! filesink location='<path>'
//   udpsrc port=<audio> caps='..OPUS..'
//     ! rtpjitterbuffer ! rtpopusdepay ! opusdec ! audioconvert ! audioresample
//     ! avenc_aac ! aacparse ! queue ! muxer.audio_0
//   udpsrc port=<video> caps='..VP8..'
//     ! rtpjitterbuffer ! rtpvp8depay ! vp8dec ! videoconvert
//     ! x264enc tune=zerolatency speed-preset=ultrafast ! video/x-h264,profile=baseline,stream-format=avc
//     ! h264parse ! queue ! muxer.video_0
//
//   VP8 and Opus are decoded to raw, then re-encoded to H.264/AAC for MP4 compatibility.

const (
	BaseRTPAudioPort = 5000
	BaseRTPVideoPort = 5002
	PortStride       = 4
)

type Recorder struct {
	RoomID       string
	Dir          string
	FilePath     string
	RTPAudioPort int
	RTPVideoPort int

	cmd     *exec.Cmd
	started bool
}

func portForRoom(roomID string, base int) int {
	var h int
	for _, c := range roomID {
		h = (h*31 + int(c)) & 0xFFFF
	}
	return base + (h%500)*PortStride
}

// NewRecorder prepares the output directory and GStreamer command.
//   - outputDir  : save directory; defaults to "./recordings/<roomID>"
//   - fileName   : base name (.mp4 appended if missing); defaults to "<roomID>_<ts>"
//   - roomID     : used for port assignment and fallback naming
//
// Uses MP4 container with H.264 video and AAC audio (transcoded from VP8/Opus).
func NewRecorder(outputDir, fileName, roomID string) (*Recorder, error) {
	if outputDir == "" {
		outputDir = filepath.Join("recordings", roomID)
	}
	if err := os.MkdirAll(outputDir, 0755); err != nil {
		return nil, fmt.Errorf("mkdir %s: %w", outputDir, err)
	}

	if fileName == "" {
		ts := time.Now().Format("2006-01-02_15-04-05")
		fileName = roomID + "_" + ts
	}
	fileName = filepath.Base(fileName)
	// Accept legacy .webm names — upgrade silently
	if strings.HasSuffix(strings.ToLower(fileName), ".webm") {
		fileName = fileName[:len(fileName)-5]
	}
	if !strings.HasSuffix(strings.ToLower(fileName), ".mp4") {
		fileName += ".mp4"
	}

	filePath := filepath.Join(outputDir, fileName)
	audioPort := portForRoom(roomID, BaseRTPAudioPort)
	videoPort := portForRoom(roomID, BaseRTPVideoPort)

	pipeline := buildPipeline(audioPort, videoPort, filePath)
	cmd := exec.Command("sh", "-c", "gst-launch-1.0 -e "+pipeline)
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr

	rec := &Recorder{
		RoomID:       roomID,
		Dir:          outputDir,
		FilePath:     filePath,
		RTPAudioPort: audioPort,
		RTPVideoPort: videoPort,
		cmd:          cmd,
	}

	if err := rec.Start(); err != nil {
		log.Printf("recorder: GStreamer unavailable for room %s: %v", roomID, err)
		log.Printf("recorder: install gst-plugins-good gst-plugins-bad gst-libav")
		log.Printf("recorder: audio port=%d  video port=%d  output=%s",
			audioPort, videoPort, filePath)
	}
	return rec, nil
}

// buildPipeline returns the gst-launch-1.0 pipeline string.
//
// Key decisions:
//   - fragment-duration=500   → 500 ms fragments; file is playable even if
//     the process exits without an EOS flush.
//   - sync=false on udpsrc    → live RTP source; disable GStreamer clock sync
//     so the pipeline never stalls on PTS.
//   - aacparse after avenc_aac → ensures properly framed AAC before mp4mux.
//   - h264parse after x264enc  → normalises the H.264 bitstream for mp4mux.
//   - "muxer." (no pad suffix) → gst-launch auto-requests the correct
//     audio_%u / video_%u sink pads on mp4mux.
// buildPipeline returns an MP4 recording pipeline with transcoding.
// VP8 -> raw -> H.264, Opus -> raw -> AAC
func buildPipeline(audioPort, videoPort int, outputPath string) string {
	mux := fmt.Sprintf(
		"mp4mux name=muxer fragment-duration=500 faststart=true ! filesink location='%s'",
		outputPath,
	)

	// Audio: RTP Opus -> depay -> decode -> AAC encode -> mux
	audio := fmt.Sprintf(
		"udpsrc port=%d"+
			" caps='application/x-rtp,media=audio,clock-rate=48000,encoding-name=OPUS,payload=111'"+
			" ! rtpjitterbuffer latency=200"+
			" ! rtpopusdepay ! opusdec"+
			" ! audioconvert ! audioresample"+
			" ! avenc_aac bitrate=128000 ! aacparse"+
			" ! queue max-size-time=100000000 leaky=downstream"+
			" ! muxer.audio_0",
		audioPort,
	)

	// Video: RTP VP8 -> depay -> decode -> H.264 encode -> mux
	video := fmt.Sprintf(
		"udpsrc port=%d"+
			" caps='application/x-rtp,media=video,clock-rate=90000,encoding-name=VP8,payload=96'"+
			" ! rtpjitterbuffer latency=200"+
			" ! rtpvp8depay ! vp8dec ! videoconvert"+
			" ! x264enc tune=zerolatency speed-preset=ultrafast key-int-max=60"+
			" ! video/x-h264,profile=baseline,stream-format=avc"+
			" ! h264parse"+
			" ! queue max-size-time=100000000 leaky=downstream"+
			" ! muxer.video_0",
		videoPort,
	)

	return fmt.Sprintf("%s %s %s", mux, audio, video)
}

// Start launches the GStreamer process.
func (r *Recorder) Start() error {
	if r.started {
		return nil
	}
	if err := r.cmd.Start(); err != nil {
		return fmt.Errorf("gst-launch-1.0: %w", err)
	}
	r.started = true
	log.Printf("recorder: started  room=%s  audio=%d  video=%d  -> %s",
		r.RoomID, r.RTPAudioPort, r.RTPVideoPort, r.FilePath)

	go func() {
		if err := r.cmd.Wait(); err != nil {
			log.Printf("recorder: pipeline exited  room=%s: %v", r.RoomID, err)
		} else {
			log.Printf("recorder: pipeline finished cleanly  room=%s", r.RoomID)
		}
	}()
	return nil
}

// Stop sends SIGINT which triggers GStreamer's EOS flush via the -e flag,
// ensuring the final MP4 fragment is written and the file is closed cleanly.
func (r *Recorder) Stop() {
	if !r.started || r.cmd == nil || r.cmd.Process == nil {
		return
	}
	if err := r.cmd.Process.Signal(os.Interrupt); err != nil {
		log.Printf("recorder: signal error  room=%s: %v -- killing", r.RoomID, err)
		_ = r.cmd.Process.Kill()
	}
	log.Printf("recorder: stopped  room=%s  file=%s", r.RoomID, r.FilePath)
}
