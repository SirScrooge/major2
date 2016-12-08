all: server client

server: svrMajor2.c
		gcc -o server svrMajor2.c -lpthread

client: cliMajor2.c
		gcc -o client cliMajor2.c

clean:
		rm -rf *.o $(objects) server client
