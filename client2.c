#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>


void error(const char *msg)
{
	perror(msg);
	exit(0);
}

void clientServer1(int portno);

int main(int argc, char *argv[])
{
	int sockfd, svr_portno, cli_1portno, cli_2portno, n, total;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[256];
	int user_entry; // value user enters into client
	int entryLog[100];
	int numEntries;
	total = 0;
	if (argc < 6)
	{
	   fprintf(stderr,"Usage: (1)%s (2)rem_ipaddr (3)svr_port (4)cli_1port (5)cli_2port (6)CPU_limit\n", argv[0]);
	   exit(1);
	}
	//Client connection
	/*
	char client1[10] = "./client1";	
	char client2[10] = "./client2";
	pthread_t t;	

	if(strcmp(argv[0], client1) == 0)
	{
		cli_1portno = atoi(argv[3]);
		if(pthread_create(&t, NULL, clientServer1, cli_1portno) < 0 )
		{
			perror("Thread failure");
			exit(1);
		}
	}
	else if(strcmp(argv[0], client2) == 0)
	{

		cli_2portno = atoi(argv[4]);
		if(pthread_create(&t, NULL, clientServer1, cli_2portno) < 0 )
		{
			perror("Thread failure");
			exit(1);
		}
	}
	else
	{
	   fprintf(stderr,"Usage: (1)%s (2)rem_ipaddr (3)svr_port (4)cli_1port (5)cli_2port (6)CPU_limit\n", argv[0]);
	   exit(1);
	}
*/
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

	//system("ps -C cli -o %cpu"); does not work i believe

	//write(sockfd, &hostname, sizeof(hostname));

	while( 1 )
	{
		printf("Current total: %d", ntohl(total));
		printf(" Enter Client Data: ");
		scanf("%d", &user_entry);
		user_entry = htonl(user_entry);
		if (user_entry == 0)
		{
			write(sockfd, &user_entry, sizeof(user_entry));
			/*if(strcmp(argv[0], client1) == 0)
			{
				cli_2portno = atoi(argv[4]);
				
			}
			else if(strcmp(argv[0], client2) == 0)
			{

				cli_1portno = atoi(argv[3]);
			}*/
			printf("Client disconnecting...\n");
			return 0;
		}
		else{
			entryLog[numEntries] = user_entry;
			numEntries++;
			write(sockfd, &user_entry, sizeof(user_entry));
			read(sockfd, &total, sizeof(total));
		}

		/*if (CPU_limit == limit)
		{
			printf("CPU Utilization: %f. Threshold %d Exceeded.\n", limit, CPU_limit);
		}*/
	}

	return 0;
}

void clientServer1(int portno)
{
	
	int cliSockfd;
	struct sockaddr_in cli_addr;
	
	struct hostent *client;
	if((cliSockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		error("ERROR opening socket");
		exit(1);
	}
	bzero((char *) & cli_addr, sizeof(cli_addr));
	cli_addr.sin_family = AF_INET;
	cli_addr.sin_addr.s_addr = INADDR_ANY;
	cli_addr.sin_port = htons(portno);
	if (bind(cliSockfd, (struct sockaddr *) &cli_addr, sizeof(cli_addr)) < 0)
	{	
		perror("ERROR on binding");
		exit(1);
	}
	listen(cliSockfd,5);
	socklen_t clilen = sizeof(cli_addr);
	accept(cliSockfd, (struct sockaddr *) &cli_addr, &clilen);
}
