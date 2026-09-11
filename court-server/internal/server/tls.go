package server

import (
	"court-server/internal/config"
	"court-server/internal/tlsutil"
)

func resolveCert(cfg *config.Config) (string, string, error) {
	return tlsutil.EnsureCert(cfg.CertFile, cfg.KeyFile)
}
