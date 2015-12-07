/*
* File:   send_client_thread.c
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
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <glib.h>

#include "multicast_server.h"

/* Local function prototypes ------------------------------------------------*/
static Msg *make_Msg(char *m);
static void prt(gpointer item);

void *send_client_thread(void *cc) {

    /* Cast argument to structure holding client info */
	ClientConn cliInfo = *(ClientConn *) cc;
	printf("Send Client Handler: %d\n", cliInfo.fd);
    printf("Send Client IP: %s\n", cliInfo.c_addr);
    printf("Send Client Port: %d\n", cliInfo.c_port);
    int sock = cliInfo.fd;

	/* Read text from socket into local buffer */
    char buf[MAXDATASIZE];  /* Message received from client */
    char msg[MAXMSGSIZE];   /* Message prefixed with IP/port info */
    int nbytes;

    /* Main loop for receiving messages from sender clients  */
    for(;;) {

        /* Receive message and store in buf */
        if ((nbytes = recv(sock, buf, sizeof buf, 0)) <= 0) {
            /* Got error or connection closed by client */
            if (nbytes == 0) {
                /* Connection closed */
                printf("server: socket %d hung up\n", sock);
            } else {
                perror("recv");
            }
            close(sock);
            pthread_exit(NULL);
        }
        /* Null terminate string and print confirmation message to server */
        buf[nbytes] = '\0';
        printf("server received: %s", buf);
        /* Create string with "IPaddr, portNum: message" prefix */ 
        sprintf(msg, "%s, %d: %s", cliInfo.c_addr, cliInfo.c_port, buf);
        
        /* Aquire message queue lock */
        if (pthread_mutex_lock(&q_lock) != 0) {
            perror("pthread_mutex_lock");
            pthread_exit(NULL);
        }

        /* Discard message if no recv clients to send to*/
        if(num_recv_clients == 0) {
            /* Release lock and continue */
            if (pthread_mutex_unlock(&q_lock) != 0) {
                perror("pthread_mutex_unlock");
                pthread_exit(NULL);
            }           
            continue;
        }
        else {
            /* Update state variables */
            seq_num++;
            num_msgs++;
            /* Add message to the message queue */
            g_queue_push_tail(msg_q, make_Msg(msg));
            /* Broadcast on condition variable for a new message */
            if (pthread_cond_broadcast(&new_msg) != 0) {
                perror("pthread_cond_broadcast");
                pthread_exit(NULL);
            }
            /* Print state of message queue to server for testing */
            printf("Start of Current Message Queue\n");
            printf("------------------------------\n");
            /* Print contents of each node in queue */
            g_queue_foreach(msg_q, (GFunc)prt, NULL);
            printf("End of Current Message Queue\n");
        }

        /* Release message queue lock */
        if (pthread_mutex_unlock(&q_lock) != 0) {
            perror("pthread_mutex_unlock");
            pthread_exit(NULL);
        }              
    }

    close(sock);
	pthread_exit(NULL);
}

/* Description: Allocate and initialize new Msg struct to store in msg_q
*  Inputs: m - pointer to message string
*
*  Return: Pointer to newly allocated Msg struct 
*/
static Msg *
make_Msg(char *m){
	Msg *message = g_new(Msg, 1);
    message->msg = strndup(m, MAXMSGSIZE);
	message->seq  = seq_num;
	message->numC = num_recv_clients;
	return message;
}

/* Description: Print all fields of a given Msg structure to server stdou
 * Inputs: item - an Msg from the message queue
 *
 * Return: none
 */
static void
prt(gpointer item) {
    printf("%sSequence number: %d\nNumber recipients: %d\n",
        ((Msg*)item)->msg, ((Msg*)item)->seq, ((Msg*)item)->numC);
    printf("------------------------------\n");
}
