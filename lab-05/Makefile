client:
	gcc src/client.c -Iinclude -o build/client

server:
	gcc src/server.c -Iinclude -o build/server

test: client server
	./build/server &
	./build/client