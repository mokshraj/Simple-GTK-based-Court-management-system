// Package config holds the runtime configuration for court-server.
package config

import (
	"flag"
	"fmt"
	"log"
	"os"
	"path/filepath"
)

// Config is the resolved configuration used to start the server.
type Config struct {
	// Network
	Addr     string // default ":1753"
	CertFile string // resolved by tlsutil.EnsureCert
	KeyFile  string // resolved by tlsutil.EnsureCert

	// Paths
	WorkDir       string // base directory for relative paths; default "."
	GUIAppPath    string // default "./gui"
	RecordingsDir string // default "./recordings"
	SiteDir       string // default "./website"
	MeetDir       string // default "./meet-server/static"
	PortalDir     string // default "./court-server/web/portal"
	CertsDir      string // default "./certs"
	JudgmentDB    string // default "./Judgment.db"

	// Portal
	BroadwayStart int // default 8090
	MaxBroadway   int // default 50

	// Behavior
	EnableTUI bool   // default auto-detect from TTY
	BaseURL   string // default auto-derive from Addr
}

// Default returns a Config populated with sensible defaults.
func Default() *Config {
	cwd, _ := os.Getwd()
	return &Config{
		Addr:          ":1753",
		WorkDir:       cwd,
		GUIAppPath:    "./gui",
		RecordingsDir: "./recordings",
		SiteDir:       "./website",
		MeetDir:       "./meet-server/static",
		PortalDir:     "./court-server/web/portal",
		CertsDir:      "./certs",
		JudgmentDB:    "./Judgment.db",
		BroadwayStart: 8090,
		MaxBroadway:   50,
		EnableTUI:     isTerminal(os.Stdin),
	}
}

// BindFlags attaches CLI flags to fs and returns the parsed values merged
// onto the receiver.
func (c *Config) BindFlags(fs *flag.FlagSet) {
	fs.StringVar(&c.Addr, "addr", c.Addr, "listen address (e.g. :1753)")
	fs.StringVar(&c.CertFile, "tls-cert", "", "TLS cert path (default: auto-resolve)")
	fs.StringVar(&c.KeyFile, "tls-key", "", "TLS key path (default: auto-resolve)")
	fs.StringVar(&c.WorkDir, "workdir", c.WorkDir, "base directory for relative paths")
	fs.StringVar(&c.GUIAppPath, "gui", c.GUIAppPath, "path to compiled gui binary")
	fs.StringVar(&c.RecordingsDir, "recordings", c.RecordingsDir, "directory for meet recordings")
	fs.StringVar(&c.SiteDir, "site", c.SiteDir, "static site root (served at /site/*)")
	fs.StringVar(&c.MeetDir, "meet-dir", c.MeetDir, "meet browser UI root (served at /meet/*)")
	fs.StringVar(&c.PortalDir, "portal-dir", c.PortalDir, "portal HTML root (served at /portal/*)")
	fs.StringVar(&c.JudgmentDB, "judgmentdb", c.JudgmentDB, "path to Judgment.db (served at /site/Judgment.db)")
	fs.StringVar(&c.BaseURL, "base-url", "", "external base URL (default: auto-derive from -addr)")
	fs.IntVar(&c.BroadwayStart, "broadway-start", c.BroadwayStart, "base port for gtk4-broadwayd")
	fs.IntVar(&c.MaxBroadway, "max-broadway", c.MaxBroadway, "max concurrent broadway sessions")
	// --tui / --no-tui are bound in main.go to avoid duplicate flag panic.
}

// Apply changes the process CWD to WorkDir and creates any missing directories.
// Returns an error if CWD cannot be changed or dirs cannot be created.
func (c *Config) Apply() error {
	// Resolve all relative paths against the original working directory (where
	// the binary was launched) before we chdir, so they remain valid after
	// the CWD change.
	origWD, _ := os.Getwd()
	if origWD == "" {
		origWD = "."
	}
	if !filepath.IsAbs(c.GUIAppPath) {
		c.GUIAppPath = filepath.Join(origWD, c.GUIAppPath)
	}
	if !filepath.IsAbs(c.PortalDir) {
		c.PortalDir = filepath.Join(origWD, c.PortalDir)
	}
	if !filepath.IsAbs(c.SiteDir) {
		c.SiteDir = filepath.Join(origWD, c.SiteDir)
	}
	if !filepath.IsAbs(c.MeetDir) {
		c.MeetDir = filepath.Join(origWD, c.MeetDir)
	}
	if !filepath.IsAbs(c.RecordingsDir) {
		c.RecordingsDir = filepath.Join(origWD, c.RecordingsDir)
	}
	if !filepath.IsAbs(c.CertsDir) {
		c.CertsDir = filepath.Join(origWD, c.CertsDir)
	}
	if !filepath.IsAbs(c.JudgmentDB) {
		c.JudgmentDB = filepath.Join(origWD, c.JudgmentDB)
	}

	if c.WorkDir != "" {
		if err := os.Chdir(c.WorkDir); err != nil {
			return fmt.Errorf("chdir %s: %w", c.WorkDir, err)
		}
	}
	for _, d := range []string{c.RecordingsDir, c.CertsDir} {
		if d == "" {
			continue
		}
		if err := os.MkdirAll(d, 0o755); err != nil {
			return fmt.Errorf("mkdir %s: %w", d, err)
		}
	}
	log.Printf("config: workdir=%s gui=%s portal=%s", c.WorkDir, c.GUIAppPath, c.PortalDir)
	return nil
}

func isTerminal(f *os.File) bool {
	fi, err := f.Stat()
	if err != nil {
		return false
	}
	return (fi.Mode() & os.ModeCharDevice) != 0
}
