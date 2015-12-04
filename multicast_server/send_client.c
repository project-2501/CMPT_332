/*
 * File:    send_client.c
 * Class:   CMPT 332 Assignment 4
 * Author:  Kyle and Taran
 * Date:    December 4, 2015
 *
 * Description: Client for sending messages to the multicast server
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

// REMOVE?
#define PORT "35000" // the port client will be connecting to 

// REMOVE?
#define MAXDATASIZE 1024 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
	int sockfd, numbytes;  
	char *buf;
    buf = (char *) malloc (MAXDATASIZE * sizeof(char));
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	if (argc != 3) {
	    fprintf(stderr,"usage: send_client hostname portnumber\n");
	    exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("send_client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("send_client: connect");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "send_client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("send_client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure

    /*
	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
	    perror("recv");
	    exit(1);
	}

	buf[numbytes] = '\0';

	printf("client: received '%s'\n",buf);
    */

    // send lines to server
    size_t nbytes = MAXDATASIZE-1;
    for(;;) {
        // getline
        if ((numbytes = getline(&buf, &nbytes, stdin)) == -1) {
            perror("getline");
            exit(1);
        }
        // send
        if (send(sockfd, buf, numbytes, 0) == -1) {
            perror("send");
            exit(1);
        }
    }

	close(sockfd);

	return 0;
}

