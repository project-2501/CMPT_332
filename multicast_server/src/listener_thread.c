/*
* File:   listener_thread.c
* Class:  CMPT 332 Assignment 4
* Author: Kyle and Taran
* Date:   December 03 2015
*
* Description: Listener thread for the server to accept new connections
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <pthread.h>
#include <glib.h>

#include "multicast_server.h"

/* Local function prototypes ------------------------------------------------*/
static void * get_in_addr(struct sockaddr *sa);

void * listener_thread(void *prt) {

	/* Cast the port number to be used for this thread */
	char port[8];
	sprintf(port, "%ld", (long) prt);

	/* Local thread variables */
    int sockfd, new_fd;  
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; 
    socklen_t sin_size;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;
	pthread_t tid;
	ClientConn cc;

	/* Setup of the addrinfo structure */
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

	/* Make call to getaddrinfo to try and setup socket */
    if ((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		pthread_exit(NULL);
    }

    /* Loop through results from getaddrinfo() */ 
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
			pthread_exit(NULL);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    /* Print server IP address */
    if ((long) prt == send_client_port) {
        char hostname[64];
        struct addrinfo *hostinfo;
        gethostname(hostname, sizeof hostname);
        hints.ai_family = AF_INET;
        if ((rv = getaddrinfo(hostname, NULL, &hints, &hostinfo)) != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            pthread_exit(NULL);
        }
        inet_ntop(hostinfo->ai_family,
                get_in_addr((struct sockaddr *)hostinfo->ai_addr), s, sizeof s);
        printf("Server IP: %s\n", s);
    }

    freeaddrinfo(servinfo); /* All done with this structure */

	/* Check for errors in socket initialization */
    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
		pthread_exit(NULL);
    }

	/* Put socket into listening mode */
    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
		pthread_exit(NULL);
    }

	/* Ready to start main accept() loop on socket */
    while(1) {  
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
        printf("server: got connection from %s\n", s);

		/* Launch handler for send client */
		if ((long) prt == send_client_port) {

			/* Build the ClientConn structure */
			cc.c_addr = s;
            cc.c_port = ntohs(((struct sockaddr_in *) &their_addr)->sin_port);
			cc.fd = new_fd;

			if( pthread_create(&tid, NULL, send_client_thread,
                (void*) &cc) < 0) {
				perror("could not create thread");
			}	
			pthread_detach(tid);
		}
		/* Launch handler for recv client */
		else {
			if( pthread_create(&tid, NULL, recv_client_thread,
                (void*) &new_fd) < 0) {
				perror("could not create thread");
			}	
			pthread_detach(tid);
		}
    }

	pthread_exit(NULL);
}

/* Description: Helper function to return sin_addr, either IPv4 or IPv6
 * Inputs: sa - sockaddr structure
 *
 * Return: none
 */
static void *
get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
