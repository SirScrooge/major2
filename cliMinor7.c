/* 
Name: Samuel Malenfant
Course: CSCE 3600 Dr. Thompson
Date: 11/21/16
Description: Minor 7
 Client Side of program which should somewhat randomly
 	choose to BUY or RETURN from server.
ISSUES:
	> Does not randomly decide what function
		to run a percentage of the time.
		(hard coded BUY and RETURN to server)
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#define SIZE 20

void error(const char *msg)
{
	perror(msg);
	exit(0);
}

struct ticket_info
{
	int ticket_num;
} tickets[SIZE];

void buy_func(int sockfd, int *ticket_totl);
void return_func(int sockfd, int *ticket_totl);
void display_tickets();

int main(int argc, char *argv[])
{
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	//initialize ticket list to 0
	int z = 0;
	for (z; z < SIZE; z++)
	{
		tickets[z].ticket_num = 0;
	}
	int ticket_totl = 0; // stores number of tickets currently taken by client

	char buffer[256];
	if (argc < 3)
	{
	   fprintf(stderr,"Usage: %s hostname port\n", argv[0]);
	   exit(0);
	}

	// First socket function here
	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) 
		error("ERROR opening socket");
	server = gethostbyname(argv[1]);
	if (server == NULL){
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, 
		 (char *)&serv_addr.sin_addr.s_addr,
		 server->h_length);
	serv_addr.sin_port = htons(portno);

	// Connect socket to server with connect()
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
		error("ERROR connecting");

	/*
	Send and receive data as needed below
	*/
	buy_func(sockfd, &ticket_totl);
	buy_func(sockfd, &ticket_totl);
	return_func(sockfd, &ticket_totl);
	buy_func(sockfd, &ticket_totl);
	buy_func(sockfd, &ticket_totl);
	return_func(sockfd, &ticket_totl);
	buy_func(sockfd, &ticket_totl);
	buy_func(sockfd, &ticket_totl);
	buy_func(sockfd, &ticket_totl);
	buy_func(sockfd, &ticket_totl);
	return_func(sockfd, &ticket_totl);
	buy_func(sockfd, &ticket_totl);
	buy_func(sockfd, &ticket_totl);
	return_func(sockfd, &ticket_totl);
	buy_func(sockfd, &ticket_totl);
	buy_func(sockfd, &ticket_totl);
	buy_func(sockfd, &ticket_totl);
	buy_func(sockfd, &ticket_totl);
	return_func(sockfd, &ticket_totl);
	buy_func(sockfd, &ticket_totl);

	/* Display list of tickets in client*/
	display_tickets();

	close(sockfd);
	return 0;
}

void buy_func(int sockfd, int *ticket_totl)
{
	int n;
	char buffer[256];
	printf("[CLIENT] : BUY\n");
	// Send request to server
	bzero(buffer,256);
	strcpy(buffer, "BUY");
	n = write(sockfd,buffer,strlen(buffer));
	if (n < 0) 
		 error("ERROR writing to socket");

	// Read value in from server
	bzero(buffer,256);
	n = read(sockfd,buffer,255);
	if (n < 0) 
		 error("ERROR reading from socket");

	// insert ticket value into location
	int i = 0;
	int temp_num;
	temp_num = atoi(buffer);
	for (i; i < SIZE; i++)
	{
		if (tickets[i].ticket_num == 0)
		{
			tickets[i].ticket_num = temp_num;
			*ticket_totl = *ticket_totl + 1;
			printf("[SERVER] : %d\n", temp_num);
			return;
		}
	}
}

void return_func(int sockfd, int *ticket_totl)
{
	int n, temp_num;
	char buffer[256];
	bzero(buffer,256);
	if (ticket_totl == 0)
	{
		time_t t;
		srand(time(&t));
		temp_num = rand() % 90000 + 10000;
	}
	else
	{
		int i = 0;
		for (i; i < SIZE; i++)
		{
			if (tickets[i].ticket_num != 0)
			{
				temp_num = tickets[i].ticket_num;
			}
		}
	}

	sprintf(buffer, "RETURN %d", temp_num);
	printf("[CLIENT] : RETURN %d\n", temp_num);
	// Send request to server
	n = write(sockfd,buffer,strlen(buffer));
	if (n < 0) 
		 error("ERROR writing to socket");

	// Read value in from server
	bzero(buffer,256);
	n = read(sockfd,buffer,255);
	if (n < 0) 
		 error("ERROR reading from socket");

	// remove ticket value from location if SUCCESS
	if (strcmp(buffer,"SUCCESS") == 0)
	{
		int i = 0;
		for (i; i < SIZE; i++)
		{
			if (tickets[i].ticket_num == temp_num)
			{
				tickets[i].ticket_num = 0;
				*ticket_totl = *ticket_totl - 1;
				printf("[CLIENT] : Ticket %d RETURN\n", temp_num);
				return;
			}
		}
	}
	else
	{
		printf("[CLIENT] : RETURN failed\n");
	}
}

void display_tickets()
{
	int i = 0;
	printf("[CLIENT] : Ticket List\n");
	printf("- - - - - - - - - - - -\n");
	for (i; i < SIZE; i++)
	{
		printf("[Tkt# %d] %d\n", i, tickets[i].ticket_num);
	} // end for loop
	printf("- - - - - - - - - - -\n");
}