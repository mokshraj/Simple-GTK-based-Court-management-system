// DEPRECATED: this binary has been merged into court-server (../court-server).
//
// To run the full stack (Broadway portal + meet + static site + TUI), from
// the workspace root:
//
//	go run ./court-server/cmd/court-server
//
// Or for headless:
//
//	go run ./court-server/cmd/court-server --no-tui
package main

import "fmt"

func main() {
	fmt.Println("meet-server has been merged into court-server.")
	fmt.Println("Run from the workspace root:  go run ./court-server/cmd/court-server")
}
