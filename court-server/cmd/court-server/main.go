// court-server is the unified Go binary that replaces the original
// Go_backend (Broadway portal on :1753) and meet-server (TLS video on
// :1755), and additionally serves the website/ static site live from disk.
//
// Run it from the workspace root so the relative paths (./website, ./gui,
// ./meet-server/static) resolve correctly. Override any of them with flags.
package main

import (
	"context"
	"flag"
	"fmt"
	"log"
	"os"
	"os/signal"
	"syscall"

	"court-server/internal/config"
	"court-server/internal/procman"
	"court-server/internal/server"
	"court-server/internal/tui"
)

func main() {
	cfg := config.Default()
	fs := flag.NewFlagSet("court-server", flag.ExitOnError)
	cfg.BindFlags(fs)
	// Override: --no-tui should disable TUI (handled in main, not in BindFlags).
	noTUI := fs.Bool("no-tui", false, "run server only (no interactive TUI)")
	// Hide the auto-registered --tui / --no-tui=false aliases from BindFlags.
	// (They were placeholders; the real one is --no-tui above.)
	_ = fs.Parse(os.Args[1:])

	if *noTUI {
		cfg.EnableTUI = false
	}

	if err := cfg.Apply(); err != nil {
		log.Fatalf("config: %v", err)
	}

	pm := procman.NewManager()
	srv, err := server.New(cfg, pm)
	if err != nil {
		log.Fatalf("server: %v", err)
	}

	ctx, cancel := signal.NotifyContext(context.Background(),
		os.Interrupt, syscall.SIGTERM)
	defer cancel()

	// Start the HTTPS server.
	errCh := make(chan error, 1)
	go func() {
		errCh <- srv.Run(ctx)
	}()

	if !cfg.EnableTUI {
		// Headless: wait for ctx or server error.
		select {
		case <-ctx.Done():
			log.Printf("main: signal received, shutting down")
		case err := <-errCh:
			if err != nil {
				log.Printf("main: server error: %v", err)
			}
		}
		srv.Stop()
		return
	}

	// Interactive: run TUI in foreground; server is in background goroutine.
	app := tui.New(srv)

	// Stop the server if the TUI exits.
	go func() {
		<-ctx.Done()
		app.Application.Stop()
	}()

	if err := app.Run(ctx); err != nil {
		log.Printf("tui: %v", err)
	}
	srv.Stop()
	fmt.Println("court-server: bye")
}
