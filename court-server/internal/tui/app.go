// Package tui implements the rivo/tview-based interactive control panel for
// court-server. It runs the HTTPS server in parallel via a goroutine.
package tui

import (
	"context"
	"fmt"
	"net"
	"os"
	"os/exec"
	"path/filepath"
	"strconv"
	"strings"
	"time"

	"court-server/internal/procman"
	"court-server/internal/server"

	"github.com/gdamore/tcell/v2"
	"github.com/rivo/tview"
)

// App is the rivo/tview application.
type App struct {
	*tview.Application
	pages    *tview.Pages
	dash     *tview.TextView
	broadway *tview.Table
	meet     *tview.Table
	recs     *tview.Table
	logs     *tview.TextView
	status   *tview.TextView
	footer   *tview.TextView
	tabBar   *tview.TextView
	control  *tview.Form
	tabNames []string
	curTab   int

	pm  *procman.Manager
	srv *server.Server

	// selection tracking for tables
	broadwaySelRow int
	meetSelRow     int
	recsSelRow     int
}

// New creates the TUI shell.
func New(srv *server.Server) *App {
	a := &App{
		Application: tview.NewApplication(),
		pages:       tview.NewPages(),
		pm:          srv.PM,
		srv:         srv,
		tabNames:    []string{"Dashboard", "Broadway", "Meet", "Recordings", "Logs", "Settings", "Control"},
	}
	a.build()
	return a
}

func (a *App) build() {
	// Header
	a.status = tview.NewTextView().
		SetDynamicColors(true).
		SetTextAlign(tview.AlignLeft)
	a.status.SetBorder(true).SetTitle(" court-server ")

	// Tab bar
	a.tabBar = tview.NewTextView().SetDynamicColors(true)
	a.tabBar.SetBackgroundColor(tcell.ColorDarkSlateGray)
	a.tabBar.SetText(a.tabBarText())

	// Pages
	a.dash = a.makeDashboard()
	a.broadway = a.makeBroadway()
	a.meet = a.makeMeet()
	a.recs = a.makeRecordings()
	a.logs = a.makeLogs()
	settings := a.makeSettings()
	a.control = a.makeControl()

	a.pages.AddPage(a.tabNames[0], a.dash, true, true)
	a.pages.AddPage(a.tabNames[1], a.broadway, true, false)
	a.pages.AddPage(a.tabNames[2], a.meet, true, false)
	a.pages.AddPage(a.tabNames[3], a.recs, true, false)
	a.pages.AddPage(a.tabNames[4], a.logs, true, false)
	a.pages.AddPage(a.tabNames[5], settings, true, false)
	a.pages.AddPage(a.tabNames[6], a.control, true, false)

	// Footer
	a.footer = tview.NewTextView().
		SetDynamicColors(true).
		SetText(" [yellow]q[-] quit  [yellow]Tab[-]/[yellow]<-[-]/[yellow]->[-] cycle  [yellow]1-7[-] jump  [yellow]r[-] refresh  [yellow]?[-] help")

	body := tview.NewFlex().SetDirection(tview.FlexRow).
		AddItem(a.status, 3, 0, false).
		AddItem(a.tabBar, 1, 0, false).
		AddItem(a.pages, 0, 1, true).
		AddItem(a.footer, 1, 0, false)

	a.SetRoot(body, true).EnableMouse(true)
	a.SetInputCapture(a.onKey)
}

func (a *App) tabBarText() string {
	s := ""
	for i, n := range a.tabNames {
		if i == a.curTab {
			s += fmt.Sprintf("[::b][black:steelblue] %d %s [-:-:-] ", i+1, n)
		} else {
			s += fmt.Sprintf("[::d] %d %s [-] ", i+1, n)
		}
	}
	return s
}

func (a *App) onKey(ev *tcell.EventKey) *tcell.EventKey {
	switch ev.Key() {
	case tcell.KeyTab:
		a.cycleTab(1)
		return nil
	case tcell.KeyBacktab:
		a.cycleTab(-1)
		return nil
	case tcell.KeyLeft:
		if a.curTab > 0 {
			a.switchTab(a.curTab - 1)
		}
		return nil
	case tcell.KeyRight:
		if a.curTab < len(a.tabNames)-1 {
			a.switchTab(a.curTab + 1)
		}
		return nil
	}
	switch ev.Rune() {
	case 'q', 'Q':
		a.Application.Stop()
		return nil
	case '1', '2', '3', '4', '5', '6', '7':
		idx := int(ev.Rune() - '1')
		if idx < len(a.tabNames) {
			a.switchTab(idx)
		}
	case 'r', 'R':
		a.Refresh()
	}
	return ev
}

func (a *App) switchTab(index int) {
	if index < 0 || index >= len(a.tabNames) {
		return
	}
	a.curTab = index
	a.pages.SwitchToPage(a.tabNames[a.curTab])
	a.refreshTabBar()
	a.setTabFocus(a.curTab)
}

func (a *App) cycleTab(dir int) {
	a.switchTab((a.curTab + dir + len(a.tabNames)) % len(a.tabNames))
}

func (a *App) setTabFocus(index int) {
	switch index {
	case 1:
		if a.broadway != nil {
			a.broadway.SetSelectable(true, false)
			if a.broadway.GetRowCount() > 1 {
				row := a.broadwaySelRow
				if row >= a.broadway.GetRowCount() {
					row = a.broadway.GetRowCount() - 1
				}
				if row < 1 {
					row = 1
				}
				a.broadway.Select(row, 0)
			}
			a.SetFocus(a.broadway)
		}
	case 2:
		if a.meet != nil {
			a.meet.SetSelectable(true, false)
			if a.meet.GetRowCount() > 1 {
				row := a.meetSelRow
				if row >= a.meet.GetRowCount() {
					row = a.meet.GetRowCount() - 1
				}
				if row < 1 {
					row = 1
				}
				a.meet.Select(row, 0)
			}
			a.SetFocus(a.meet)
		}
	case 3:
		if a.recs != nil {
			a.recs.SetSelectable(true, false)
			if a.recs.GetRowCount() > 1 {
				row := a.recsSelRow
				if row >= a.recs.GetRowCount() {
					row = a.recs.GetRowCount() - 1
				}
				if row < 1 {
					row = 1
				}
				a.recs.Select(row, 0)
			}
			a.SetFocus(a.recs)
		}
	case 4:
		if a.logs != nil {
			a.SetFocus(a.logs)
		}
	case 5:
		if a.control != nil {
			a.SetFocus(a.control)
		}
	default:
		if a.dash != nil {
			a.SetFocus(a.dash)
		}
	}
}

func (a *App) refreshTabBar() {
	if a.tabBar != nil {
		a.tabBar.SetText(a.tabBarText())
	}
}

// Run blocks until the user quits or the context is cancelled.
func (a *App) Run(ctx context.Context) error {
	go a.tick(ctx)
	if err := a.Application.Run(); err != nil {
		return err
	}
	return nil
}

func (a *App) tick(ctx context.Context) {
	t := time.NewTicker(1 * time.Second)
	defer t.Stop()
	for {
		select {
		case <-ctx.Done():
			a.Application.Stop()
			return
		case <-t.C:
			a.QueueUpdateDraw(func() { a.Refresh() })
		case <-a.srv.Portal.SessionOpen:
			a.QueueUpdateDraw(func() { a.Refresh() })
		}
	}
}

// Refresh updates every tab's data.
func (a *App) Refresh() {
	a.refreshStatus()
	a.refreshBroadway()
	a.refreshMeet()
	a.refreshRecordings()
	a.refreshLogs()
}

func (a *App) refreshStatus() {
	rooms := a.srv.RM.ListRooms()
	procs := a.pm.Snapshot()
	a.status.SetText(fmt.Sprintf(" addr=%s  rooms=%d  procs=%d",
		a.srv.Cfg.Addr, len(rooms), len(procs),
	))
}

func (a *App) refreshBroadway() {
	if a.broadway == nil {
		return
	}
	selRow := 1
	_, rowsSelectable := a.broadway.GetSelectable()
	if rowsSelectable {
		row, _ := a.broadway.GetSelection()
		if row > 0 {
			selRow = row
		}
	}
	a.broadway.Clear()
	headers := []string{"ID", "Kind", "PID", "Uptime", "Meta"}
	for i, h := range headers {
		a.broadway.SetCell(0, i, tview.NewTableCell(h).SetSelectable(false).SetExpansion(1))
	}
	row := 1
	for _, p := range a.pm.Snapshot() {
		if p.Kind != "broadway" && p.Kind != "gui" {
			continue
		}
		a.broadway.SetCell(row, 0, tview.NewTableCell(p.ID))
		a.broadway.SetCell(row, 1, tview.NewTableCell(p.Kind))
		a.broadway.SetCell(row, 2, tview.NewTableCell(fmt.Sprintf("%d", p.PID)))
		a.broadway.SetCell(row, 3, tview.NewTableCell(p.Uptime))
		a.broadway.SetCell(row, 4, tview.NewTableCell(fmt.Sprintf("%v", p.Meta)))
		row++
	}
	if a.broadway.GetRowCount() > 1 {
		if selRow >= a.broadway.GetRowCount() {
			selRow = a.broadway.GetRowCount() - 1
		}
		a.broadway.Select(selRow, 0)
		a.broadwaySelRow = selRow
	}
}

func (a *App) refreshMeet() {
	if a.meet == nil {
		return
	}
	selRow := 1
	_, rowsSelectable := a.meet.GetSelectable()
	if rowsSelectable {
		row, _ := a.meet.GetSelection()
		if row > 0 {
			selRow = row
		}
	}
	a.meet.Clear()
	a.meet.SetCell(0, 0, tview.NewTableCell("Room ID").SetSelectable(false).SetExpansion(1))
	row := 1
	for _, id := range a.srv.RM.ListRooms() {
		a.meet.SetCell(row, 0, tview.NewTableCell(id))
		row++
	}
	if a.meet.GetRowCount() > 1 {
		if selRow >= a.meet.GetRowCount() {
			selRow = a.meet.GetRowCount() - 1
		}
		a.meet.Select(selRow, 0)
		a.meetSelRow = selRow
	}
}

func (a *App) refreshRecordings() {
	if a.recs == nil {
		return
	}
	selRow := 1
	_, rowsSelectable := a.recs.GetSelectable()
	if rowsSelectable {
		row, _ := a.recs.GetSelection()
		if row > 0 {
			selRow = row
		}
	}
	a.recs.Clear()
	a.recs.SetCell(0, 0, tview.NewTableCell("File").SetSelectable(false).SetExpansion(1))
	row := 1
	for _, p := range listMP4s(a.srv.Cfg.RecordingsDir) {
		a.recs.SetCell(row, 0, tview.NewTableCell(p))
		row++
	}
	if a.recs.GetRowCount() > 1 {
		if selRow >= a.recs.GetRowCount() {
			selRow = a.recs.GetRowCount() - 1
		}
		a.recs.Select(selRow, 0)
		a.recsSelRow = selRow
	}
}

func (a *App) refreshLogs() {
	if a.logs == nil {
		return
	}
	a.logs.Clear()
	for id, buf := range a.pm.AllBuffers() {
		fmt.Fprintf(a.logs, "════ %s ════\n", id)
		for _, line := range buf.Tail(50) {
			fmt.Fprintln(a.logs, line)
		}
		fmt.Fprintln(a.logs)
	}
}

func (a *App) makeDashboard() *tview.TextView {
	t := tview.NewTextView().SetDynamicColors(true).SetScrollable(true)
	t.SetBorder(true).SetTitle(" Dashboard ")
	fmt.Fprintln(t, "[green]Court-server running.[-]")
	fmt.Fprintln(t, "Visit the site at /site/, the meet UI at /meet/,")
	fmt.Fprintln(t, "and the portal at /portal/.")
	return t
}

func (a *App) makeBroadway() *tview.Table {
	t := tview.NewTable().SetSelectable(true, false)
	t.SetBorder(true).SetTitle(" Broadway sessions ")
	t.SetSelectionChangedFunc(func(row, column int) {
		if row > 0 {
			a.broadwaySelRow = row
		}
	})
	return t
}

func (a *App) makeMeet() *tview.Table {
	t := tview.NewTable().SetSelectable(true, false)
	t.SetBorder(true).SetTitle(" Meet rooms ")
	t.SetSelectionChangedFunc(func(row, column int) {
		if row > 0 {
			a.meetSelRow = row
		}
	})
	return t
}

func (a *App) makeRecordings() *tview.Table {
	t := tview.NewTable().SetSelectable(true, false)
	t.SetBorder(true).SetTitle(" Recordings ")
	t.SetSelectionChangedFunc(func(row, column int) {
		if row > 0 {
			a.recsSelRow = row
		}
	})
	return t
}

func (a *App) makeLogs() *tview.TextView {
	t := tview.NewTextView().SetDynamicColors(true).SetScrollable(true)
	t.SetBorder(true).SetTitle(" Logs (last 50 lines / process) ")
	return t
}

func (a *App) makeSettings() tview.Primitive {
	form := tview.NewForm()
	form.AddInputField("Addr", a.srv.Cfg.Addr, 20, nil, nil)
	form.AddInputField("GUI", a.srv.Cfg.GUIAppPath, 40, nil, nil)
	form.AddInputField("Site", a.srv.Cfg.SiteDir, 40, nil, nil)
	form.AddInputField("Meet", a.srv.Cfg.MeetDir, 40, nil, nil)
	form.AddButton("Quit", func() { a.Application.Stop() })
	form.SetBorder(true).SetTitle(" Settings (read-only) ")
	return form
}

func (a *App) makeControl() *tview.Form {
	form := tview.NewForm()
	form.AddButton("Start Broadway", a.actionStartBroadway)
	form.AddButton("Stop Broadway", a.actionStopBroadway)
	form.AddButton("Start Meet", a.actionStartMeet)
	form.AddButton("Stop Meet", a.actionStopMeet)
	form.AddButton("Build / Rebuild", a.actionBuild)
	form.AddButton("Quit", func() { a.Application.Stop() })
	form.SetBorder(true).SetTitle(" Control ")
	return form
}

func (a *App) actionStartBroadway() {
	go func() {
		if err := a.startBroadway(); err != nil {
			a.QueueUpdateDraw(func() {
				fmt.Fprintf(a.status, "[red]Start Broadway failed: %v[-]", err)
			})
		}
	}()
}

func (a *App) actionStopBroadway() {
	go func() {
		if err := a.stopBroadway(); err != nil {
			a.QueueUpdateDraw(func() {
				fmt.Fprintf(a.status, "[red]Stop Broadway failed: %v[-]", err)
			})
		}
	}()
}

func (a *App) actionStartMeet() {
	go func() {
		if err := a.startMeet(); err != nil {
			a.QueueUpdateDraw(func() {
				fmt.Fprintf(a.status, "[red]Start Meet failed: %v[-]", err)
			})
		}
	}()
}

func (a *App) actionStopMeet() {
	go func() {
		if err := a.stopMeet(); err != nil {
			a.QueueUpdateDraw(func() {
				fmt.Fprintf(a.status, "[red]Stop Meet failed: %v[-]", err)
			})
		}
	}()
}

func (a *App) actionBuild() {
	go func() {
		if err := a.buildServer(); err != nil {
			a.QueueUpdateDraw(func() {
				fmt.Fprintf(a.status, "[red]Build failed: %v[-]", err)
			})
		}
	}()
}

// buildServer runs `go build ./cmd/court-server` from the court-server directory.
func (a *App) buildServer() error {
	ctx, cancel := context.WithTimeout(context.Background(), 2*time.Minute)
	defer cancel()
	cmd := exec.CommandContext(ctx, "go", "build", "-o", "court-server", "./cmd/court-server")
	cmd.Dir = "court-server"
	cmd.Env = append(os.Environ(), "GO111MODULE=on")
	out, err := cmd.CombinedOutput()
	if err != nil {
		a.QueueUpdateDraw(func() {
			fmt.Fprintf(a.status, "[red]Build failed: %v[-]\n%s", err, string(out))
		})
		return err
	}
	a.QueueUpdateDraw(func() {
		fmt.Fprintln(a.status, "[green]Build succeeded.[-]")
	})
	return nil
}

// startBroadway spawns the GUI Broadway session using the configured GUI app.
func (a *App) startBroadway() error {
	if a.srv == nil || a.srv.Cfg == nil {
		return fmt.Errorf("server not initialized")
	}
	// Use the same session allocator as the web flow so we don't collide with
	// already-running sessions.
	displayNum, err := a.srv.Portal.Sm.GetNextID(a.srv.Portal.Reg)
	if err != nil {
		return fmt.Errorf("no free session slot: %w", err)
	}
	broadwayPort := a.srv.Cfg.BroadwayStart + displayNum
	displayStr := fmt.Sprintf(":%d", displayNum)

	// Start gtk4-broadwayd
	bwCmd := exec.Command("gtk4-broadwayd",
		"--address", "::",
		"--port", fmt.Sprintf("%d", broadwayPort),
		displayStr,
	)
	if _, err := a.pm.StartAndRegister(fmt.Sprintf("broadway:%d", displayNum), "broadway", bwCmd, map[string]string{
		"display":       displayStr,
		"broadway_port": strconv.Itoa(broadwayPort),
	}); err != nil {
		a.QueueUpdateDraw(func() {
			fmt.Fprintf(a.status, "[red]Broadway daemon start failed: %v[-]", err)
		})
		return err
	}

	// Wait for port ready
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
		a.QueueUpdateDraw(func() {
			fmt.Fprintf(a.status, "[yellow]Broadway port %d not ready yet[-]", broadwayPort)
		})
	}

	// Start GUI app
	absAppPath, _ := filepath.Abs(a.srv.Cfg.GUIAppPath)
	appCmd := exec.Command(absAppPath, "web")
	appCmd.Dir = filepath.Dir(absAppPath)
	appCmd.Env = []string{
		"GDK_BACKEND=broadway",
		"BROADWAY_DISPLAY=" + displayStr,
		"DBUS_SESSION_BUS_ADDRESS=unix:path=/dev/null",
		"XDG_RUNTIME_DIR=" + os.Getenv("XDG_RUNTIME_DIR"),
		"PATH=" + os.Getenv("PATH"),
		"SESSION_ID=" + strconv.Itoa(displayNum),
	}
	if err := appCmd.Start(); err != nil {
		a.QueueUpdateDraw(func() {
			fmt.Fprintf(a.status, "[red]GUI app start failed: %v[-]", err)
		})
		_ = a.pm.Kill(fmt.Sprintf("broadway:%d", displayNum), 2*time.Second)
		return err
	}
	guiID := fmt.Sprintf("gui:%d", displayNum)
	if _, err := a.pm.StartAndRegister(guiID, "gui", appCmd, map[string]string{
		"display": displayStr,
		"app":     absAppPath,
	}); err != nil {
		a.QueueUpdateDraw(func() {
			fmt.Fprintf(a.status, "[red]GUI register failed: %v[-]", err)
		})
		_ = a.pm.Kill(guiID, 2*time.Second)
		return err
	}

	a.QueueUpdateDraw(func() {
		fmt.Fprintf(a.status, "[green]Broadway session %d started on port %d[-]", displayNum, broadwayPort)
	})
	return nil
}

// stopBroadway stops all running Broadway/GUI processes.
func (a *App) stopBroadway() error {
	for _, p := range a.pm.Snapshot() {
		if p.Kind == "broadway" || p.Kind == "gui" {
			if err := a.pm.Kill(p.ID, 2*time.Second); err != nil {
				a.QueueUpdateDraw(func() {
					fmt.Fprintf(a.status, "[red]Stop failed for %s: %v[-]", p.ID, err)
				})
			}
		}
	}
	a.QueueUpdateDraw(func() {
		fmt.Fprintln(a.status, "[yellow]Broadway stop requested.[-]")
	})
	return nil
}

// startMeet is a placeholder for meet startup logic.
func (a *App) startMeet() error {
	a.QueueUpdateDraw(func() {
		fmt.Fprintln(a.status, "[yellow]Meet start not implemented yet.[-]")
	})
	return fmt.Errorf("not implemented")
}

// stopMeet is a placeholder for meet shutdown logic.
func (a *App) stopMeet() error {
	a.QueueUpdateDraw(func() {
		fmt.Fprintln(a.status, "[yellow]Meet stop not implemented yet.[-]")
	})
	return fmt.Errorf("not implemented")
}

// parseBuildOutput is a small helper to strip ANSI color codes from go build output.
func parseBuildOutput(s string) string {
	s = strings.ReplaceAll(s, "\x1b[32m", "")
	s = strings.ReplaceAll(s, "\x1b[31m", "")
	s = strings.ReplaceAll(s, "\x1b[33m", "")
	s = strings.ReplaceAll(s, "\x1b[0m", "")
	return s
}
