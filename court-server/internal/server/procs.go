package server

import (
	"encoding/json"
	"net/http"

	"court-server/internal/procman"
)

func writeProcs(w http.ResponseWriter, snaps []procman.Snapshot) {
	_ = json.NewEncoder(w).Encode(snaps)
}
