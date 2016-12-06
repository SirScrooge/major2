#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg)
{
	perror(msg);
	exit(0);
}

int main(int argc, char *argv[])
{
	int sockfd, svr_portno, cli_1portno, cli_2portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[256];

	if (argc < 6)
	{
	   fprintf(stderr,"Usage: (1)%s (2)rem_ipaddr (3)svr_port (4)cli_1port (5)cli_2port (6)CPU_limit\n", argv[0]);
	   exit(1);
	}

	// First socket function here to connect to server socket
	svr_portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		error("ERROR opening socket");
		exit(1);
	}

	server = gethostbyname(argv[1]); // ip adddress here
	if (server == NULL){
		fprintf(stderr,"ERROR, no such host\n");
		exit(1);
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, 
		 (char *)&serv_addr.sin_addr.s_addr,
		 server->h_length);
	serv_addr.sin_port = htons(svr_portno);

	// Connect socket to server with connect()
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
	{
		error("ERROR connecting");
	}

	while(1)
	{
		if (/* (value == 0) || (threshold <= CPU USAGE)*/)
		{
			close(sockfd);
		}
	}

	return 0;
}