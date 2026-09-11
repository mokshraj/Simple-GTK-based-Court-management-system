# court-server Makefile

BIN := court-server
PKG := ./cmd/court-server

.PHONY: build run tidy clean

build:
	go build -o $(BIN) $(PKG)

run: build
	./$(BIN)

tidy:
	go mod tidy

clean:
	rm -f $(BIN)
