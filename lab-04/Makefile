.ONESHELL:
SHELL := /bin/bash

server:
	gcc src/server.c -Iinclude -o build/server

client:
	gcc src/client.c -Iinclude -o build/client

test: client server
	PROJECT_DIR=$$(pwd)
	TEMPORARY_DIR=$$(mktemp -d)
	touch "$$TEMPORARY_DIR/notexecutable1" "$$TEMPORARY_DIR/notexecutable2" "$$TEMPORARY_DIR/executable1" "$$TEMPORARY_DIR/executable2"
	mkdir "$$TEMPORARY_DIR/directory"
	chmod +x "$$TEMPORARY_DIR/executable1" "$$TEMPORARY_DIR/executable2" "$$TEMPORARY_DIR/directory"
	chmod -x "$$TEMPORARY_DIR/notexecutable1" "$$TEMPORARY_DIR/notexecutable2"
	(cd "$$TEMPORARY_DIR" && "$$PROJECT_DIR/build/server" &)
	sleep 0.1
	"$$PROJECT_DIR/build/client"