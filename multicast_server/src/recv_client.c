/*
 * File:    recv_client.c
 * Class:   CMPT 332 Assignment 4
 * Author:  Kyle and Taran
 * Date:    December 4, 2015
 *
 * Description: Client for receiving messages from the multicast server
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAXDATASIZE 1088

/* Function Prototypes *******************************************************/
static void *get_in_addr(struct sockaddr *sa);

/* Description: Helper function to return sin_addr, either IPv4 or IPv6
 * Inputs: sa - sockaddr structure
 *
 * Return: none
 */
static void *
get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
	int sockfd, numbytes;  
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

    /* Check for correct number of arguments */
	if (argc != 3) {
	    fprintf(stderr,"usage: recv_client hostname portnumber\n");
	    exit(1);
	}

    /* Setup of the addrinfo structure */
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

    /* Make call to getaddrinfo to try and setup socket */
	if ((rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	/* Loop through all the results and connect to the first we can */
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("recv_client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("recv_client: connect");
			continue;
		}

		break;
	}

    /* Check for errors in connecting */
	if (p == NULL) {
		fprintf(stderr, "recv_client: failed to connect\n");
		return 2;
	}

    /* Print connection message with server IP */
	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("recv_client: connecting to %s\n", s);

	freeaddrinfo(servinfo); /* All done with this structure */

    /* Main loop for receiving messages */
    for(;;) {
        /* Store message in buf */
	    if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
	        perror("recv");
	        exit(1);
	    }

        /* Null terminate message */
	    buf[numbytes] = '\0';
        
        /* Print message if not empty */
        if (numbytes != 0)
	        printf("%s",buf);
    }

	close(sockfd);

	return 0;
}
