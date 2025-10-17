client:
	gcc src/client.c -Iinclude -o build/client

server:
	gcc src/server.c -Iinclude -o build/server

test: client server 
	postfix start-fg &
	
	useradd -m -s /bin/bash testuser1
	useradd -m -s /bin/bash testuser2
	useradd -m -s /bin/bash testuser3

	su - testuser1 -c 'echo "Hello, from testuser1!" | mail -s "Test email" root' 2>/dev/null
	su - testuser2 -c 'echo "Hello #1, from testuser2!" | mail -s "Test email" root' 2>/dev/null
	su - testuser2 -c 'echo "Hello #2, from testuser2!" | mail -s "Test email" root' 2>/dev/null
	su - testuser3 -c 'echo "Hello #3, from testuser3!" | mail -s "Test email" root' 2>/dev/null


	sleep 2
	
	/root/build/client &
	/root/build/server
	
	