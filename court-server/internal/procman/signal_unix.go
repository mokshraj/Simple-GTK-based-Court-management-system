//go:build unix

package procman

import (
	"os"
)

// interrupt is the polite "please shut down" signal (SIGINT on unix).
var interrupt = os.Interrupt
