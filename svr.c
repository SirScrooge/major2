#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

void census_update(int sock, int *total);

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n, pid;
    int total = 0;

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

    // Listen for a client connection
    listen(sockfd,5);
    clilen = sizeof(cli_addr);

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
            census_update(newsockfd, &total);
            exit(0);
        }
        else
        {
            close(newsockfd);
        }
    } // end while loop
}

void census_update(int sock, int *total)
{
    int n; int temp_entry = 0;
    char buffer[256];
    bzero(buffer,256);

    n = read(sock, buffer, 255);
    if (n < 0)
    {
        perror("ERROR reading from socket");
        exit(1);
    }

    temp_entry = atoi(buffer); // add if case for if temp == 0 else add?

    *total = *total + temp_entry;
    printf("%d\n", *total);

    int temp_total = 0;
    temp_total = *total;

    bzero(buffer, 256);
    itoa(temp_total, buffer, 10);

    n = write(sock, buffer, 255);
    if (n < 0)
    {
        error("ERROR writing to socket");
        exit(1);
    }
}