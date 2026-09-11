// Package tlsutil auto-generates a self-signed ECDSA certificate so the
// unified court-server can serve TLS out of the box (WebRTC, secure context).
//
// Lookup order (first existing pair wins):
//
//  1. --tls-cert / --tls-key CLI flag (caller's responsibility)
//  2. ./certs/cert.pem + ./certs/key.pem
//  3. ./cert.pem + ./key.pem
//  4. Auto-generate and persist to ./certs/{cert,key}.pem (mode 0600)
package tlsutil

import (
	"crypto/ecdsa"
	"crypto/elliptic"
	"crypto/rand"
	"crypto/x509"
	"crypto/x509/pkix"
	"encoding/pem"
	"fmt"
	"math/big"
	"net"
	"os"
	"path/filepath"
	"time"
)

// EnsureCert returns the cert and key paths to use, generating a self-signed
// ECDSA P-256 certificate if no existing pair is found. The generated cert is
// valid for ~10 years and lists every non-loopback local interface address
// plus "localhost" in the SAN, so the browser will accept it after a one-time
// "Proceed anyway" click.
func EnsureCert(explicitCert, explicitKey string) (certPath, keyPath string, err error) {
	// 1. explicit
	if explicitCert != "" && explicitKey != "" {
		if exists(explicitCert) && exists(explicitKey) {
			return explicitCert, explicitKey, nil
		}
	}

	// 2. ./certs/
	if exists("certs/cert.pem") && exists("certs/key.pem") {
		return "certs/cert.pem", "certs/key.pem", nil
	}

	// 3. ./ (legacy)
	if exists("cert.pem") && exists("key.pem") {
		return "cert.pem", "key.pem", nil
	}

	// 4. auto-generate
	if err := os.MkdirAll("certs", 0o755); err != nil {
		return "", "", fmt.Errorf("mkdir certs: %w", err)
	}
	certPath = filepath.Join("certs", "cert.pem")
	keyPath = filepath.Join("certs", "key.pem")

	if err := generate(certPath, keyPath); err != nil {
		return "", "", err
	}
	return certPath, keyPath, nil
}

func exists(p string) bool {
	_, err := os.Stat(p)
	return err == nil
}

func generate(certPath, keyPath string) error {
	priv, err := ecdsa.GenerateKey(elliptic.P256(), rand.Reader)
	if err != nil {
		return fmt.Errorf("ecdsa key: %w", err)
	}

	serialMax := new(big.Int).Lsh(big.NewInt(1), 128)
	serial, err := rand.Int(rand.Reader, serialMax)
	if err != nil {
		return fmt.Errorf("serial: %w", err)
	}

	tpl := &x509.Certificate{
		SerialNumber: serial,
		Subject:      pkix.Name{CommonName: "court-server (auto-generated)"},
		NotBefore:    time.Now().Add(-time.Hour),
		NotAfter:     time.Now().AddDate(10, 0, 0),
		KeyUsage:     x509.KeyUsageDigitalSignature | x509.KeyUsageKeyEncipherment,
		ExtKeyUsage:  []x509.ExtKeyUsage{x509.ExtKeyUsageServerAuth},
	}

	// Add every non-loopback local IP to the SAN.
	if addrs, err := net.InterfaceAddrs(); err == nil {
		for _, a := range addrs {
			if ipnet, ok := a.(*net.IPNet); ok && !ipnet.IP.IsLoopback() {
				tpl.IPAddresses = append(tpl.IPAddresses, ipnet.IP)
			}
		}
	}
	tpl.DNSNames = []string{"localhost", "ip6-localhost", "ip6-loopback"}

	der, err := x509.CreateCertificate(rand.Reader, tpl, tpl, &priv.PublicKey, priv)
	if err != nil {
		return fmt.Errorf("create cert: %w", err)
	}

	keyDER, err := x509.MarshalECPrivateKey(priv)
	if err != nil {
		return fmt.Errorf("marshal key: %w", err)
	}

	if err := writePEM(certPath, "CERTIFICATE", der); err != nil {
		return err
	}
	if err := writePEM(keyPath, "EC PRIVATE KEY", keyDER); err != nil {
		return err
	}
	_ = os.Chmod(keyPath, 0o600)
	return nil
}

func writePEM(path, blockType string, der []byte) error {
	f, err := os.OpenFile(path, os.O_CREATE|os.O_WRONLY|os.O_TRUNC, 0o600)
	if err != nil {
		return fmt.Errorf("open %s: %w", path, err)
	}
	defer f.Close()
	return pem.Encode(f, &pem.Block{Type: blockType, Bytes: der})
}
