
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

int census_update(int sock);

int total;
char hostname[128];


int main(int argc, char *argv[])
{
    gethostname(hostname, sizeof(hostname));
    printf("my hostname: %s\n", hostname);

    int sockfd, newsockfd, svr_port;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n, pid;
    total = 0;
  
    pthread_t t[2];
    
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
    svr_port = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(svr_port);

    // Bind host address
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
    sizeof(serv_addr)) < 0) 
        error("ERROR on binding");

    // Listen for a client connection
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    printf("Waiting for client(s) connection...\n");

    while(newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen))
    {
	if(pthread_create(&t[0], NULL, census_update, newsockfd) < 0)
	{
	perror("Thread failure");
	exit(1);
        }
    }	
 
	printf("ayo");


	pthread_join(t, NULL);
    

/*
    while(1)
    {
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
            // Runs client process
            close(sockfd);
            // Processing function(s) here
            while(census_update(newsockfd, &total));
            exit(0);
        }
        else
        {
            close(newsockfd);
        }

    } // end while loop*/
}


int census_update(int sock)
{
    
    while(1){
    int n; int temp_entry = 0;
 
    n = read(sock, &temp_entry, sizeof(temp_entry));
    if (n < 0)
    {
        perror("ERROR reading from socket");
        exit(1);
    }
    if(temp_entry == 0)
    {
	
	printf("Client disconnected\n");
	return 0;
    } 
    total = total + ntohl(temp_entry);
    printf("%d\n", total);

    int temp_total = 0;
    temp_total = htonl(total);
   
    n = write(sock, &temp_total, sizeof(temp_total));
    if (n < 0)
    {
        error("ERROR writing to socket");
        exit(1);
    }
    }
    close(sock);
}
