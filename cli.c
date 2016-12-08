#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>


void error(const char *msg)
{
	perror(msg);
	exit(0);
}

void send_value(int *user_entry);
long double cpu_get();

int main(int argc, char *argv[])
{
	int sockfd, svr_portno, cli_1portno, cli_2portno, n, total;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[256];
	int user_entry; // value user enters into client
	char *ptr;
	long double CPU_limit = strtod(argv[5], &ptr);
	total = 0;
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

	while( 1 )
	{
		printf("Current total: %d", ntohl(total));
		printf(" Enter Client Data: ");
		scanf("%d", &user_entry);
		user_entry = htonl(user_entry);
		if (user_entry == 0)
		{
			printf("Client disconnecting...\n");
			return 0;
		}
		else{
			write(sockfd, &user_entry, sizeof(user_entry));
			read(sockfd, &total, sizeof(total));
		}

		long double limit = cpu_get();
		if ( CPU_limit <= limit )
		{
			printf("CPU Utilization: %Lf. Threshold %Lf Exceeded.\n", limit, CPU_limit);
		}
	}

	return 0;
}

void send_value(int *user_entry)
{
	;
}


long double cpu_get()
{
    long double a[4], b[4], loadavg;
    FILE *fp;
    char dump[50];

    for(;;)
    {
        fp = fopen("/proc/stat","r");
        fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&a[0],&a[1],&a[2],&a[3]);
        fclose(fp);
        sleep(1);

        fp = fopen("/proc/stat","r");
        fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&b[0],&b[1],&b[2],&b[3]);
        fclose(fp);

        loadavg = ((b[0]+b[1]+b[2]) - (a[0]+a[1]+a[2])) / ((b[0]+b[1]+b[2]+b[3]) - (a[0]+a[1]+a[2]+a[3]));
    }

    return(loadavg);
}