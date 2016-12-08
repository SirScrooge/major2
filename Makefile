all: server client1 client2

server: svrMajor2.c
		gcc -o server svrMajor2.c

client1: cliMajor2.c
		gcc -o client1 cliMajor2.c

client2: cliMajor2.c
		gcc -o client2 cliMajor2.c

clean:
		rm -rf *.o $(objects) server client1 client2
