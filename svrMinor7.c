/* 
Name: Samuel Malenfant
Course: CSCE 3600 Dr. Thompson
Date: 11/21/16
Description: Minor 7
 Server Side of program which should accept commands from 
	two different clients at "same time".
ISSUES:
	> Does not run two clients at same time.
	> Will not exit while loop and display final tickets within server.
	> Have to use Control+C to exit program 
		without having to kill process afterwards.
	> Does not say "Client 1" or "Client 2".
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#define SIZE 25 // max amount of tickets
#define X 99999 // upper limit for ticket #

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

struct ticket_info // need variable for client ID
{
	int ticket_num; 
	int status; // 0 == AVAIL, 1 == taken
} tickets[SIZE];

int ticket_process(int socket_id);
void display_tickets();

int main(int argc, char *argv[])
{
	int sockfd, newsockfd, portno;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int n, pid;

	/* Create integer array which holds
	random and unique 5 digit ticket values
	(Partial Floyd algorithm)
	*/
	time_t t;
	srand(time(&t));
	unsigned char is_used[X] = {0}; // signals if number is used
	int in = 0, im = 0;
	for (in; in < SIZE; in++)
	{
		int r = rand() % 90000 + 10000; // for range 10000-99999
		if (is_used[r]){
			r = in;
		}
		assert(!is_used[r]); // bug checking with assert
		tickets[in].ticket_num = r; // start from 10000 for correct range
		tickets[in].status = 0;
		is_used[r] = 1;
	} // end for loop

	if (argc < 2)
	{
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}

	// First socket function here
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");

	// Initialize socket for server
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	// Bind host address
	if (bind(sockfd, (struct sockaddr *) &serv_addr,
	sizeof(serv_addr)) < 0) 
		error("ERROR on binding");

	// Display initial list of tickets
	display_tickets();
	

	// Listen for a client connection
	listen(sockfd,5);
	clilen = sizeof(cli_addr);

	/* Place accept() within infinite loop to accept
	multiple client connections. Use forking for child processes
	*/
	while(1)
	{
		// Accept client connection
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0)
			error("ERROR on accept");

		// Creation of child process
		pid = fork();
		
		if (pid < 0)
		{
			perror("ERROR forking");
			exit(1);
		}

		if (pid == 0)
		{
			int response;
			// Runs client process
			close(sockfd);
			do{
				response = ticket_process(newsockfd);
			} while (response > 0);
			close(newsockfd);
			exit(0);
		}
	} // end while loop
	display_tickets();
	close(sockfd);

	return 0; 
}

int ticket_process(int socket_id)
{
	int n;
	const char s[2] = " ";
	char *token;
	char buffer[256];
	char buffer_copy[256];
	bzero(buffer_copy,256);
	bzero(buffer,256);
	
	n = read(socket_id, buffer, 255);
	if (n < 0) error("ERROR reading from socket.");
	
	strcpy(buffer_copy, buffer);
	token = strtok(buffer_copy, s);
	// Process a BUY request
	if (strcmp(token, "BUY") == 0)
	{
		printf("[CLIENT %d] : BUY\n", socket_id);
		int i = 0;
		for (i; i < SIZE; i++)
		{
			if (tickets[i].status == 0)
			{
				char temp[7]; bzero(temp, 7);
				sprintf(temp, "%d", tickets[i].ticket_num);
				n = write(socket_id, temp, 6);
				if(n < 0) error("ERROR writing to socket.");
				tickets[i].status = 1;
				printf("[SERVER X] : Client %d BUY %d\n",
				 socket_id, tickets[i].ticket_num);
				return;
			}
		} // end for loop
	}

	strcpy(buffer_copy, buffer);
	token = strtok(buffer_copy, s);
	// Process a RETURN request
	if (strcmp(token, "RETURN") == 0)
	{
		token = strtok(NULL, s);
		int temp_num;
		temp_num = atoi(token);
		printf("[CLIENT %d] : RETURN %d\n", socket_id, temp_num);
		
		int i = 0;
		for (i; i < SIZE; i++)
		{
			if (tickets[i].ticket_num == temp_num)
			{
				tickets[i].status = 0;
				bzero(buffer_copy,256);
				strcpy(buffer_copy, "SUCCESS");
				n = write(socket_id, buffer_copy, 255);
				if(n < 0) error("ERROR writing to socket.");
				printf("[SERVER X] : Client %d RETURN %d\n", socket_id, temp_num);
				return;
			}
		}
		bzero(buffer_copy,256);
		strcpy(buffer_copy, "FAIL");
		n = write(socket_id, buffer_copy, 255);
		if(n < 0) error("ERROR writing to socket.");
		printf("[SERVER X] : Client %d RETURN %d failed\n", socket_id, temp_num);
	}
}

void display_tickets()
{
	int i = 0;
	printf("[SERVER] : Ticket List\n");
	printf("- - - - - - - - - - - -\n");
	for (i; i < SIZE; i++)
	{
		printf("[Tkt# %d] %d : ", i, tickets[i].ticket_num);
		if (tickets[i].status == 0)
		{
			printf("AVAIL\n");
		}
		if (tickets[i].status == 1)
		{
			printf("SOLD\n");
		}
	} // end for loop
	printf("- - - - - - - - - - -\n");
}