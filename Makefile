all: server client

server: server.c
		gcc -o server server.c -lpthread

client1: client.c
		gcc -o client1 client.c
		
client2: client.c
		gcc -o client2 client.c

clean:
		rm -rf *.o $(objects) server client1 client2
