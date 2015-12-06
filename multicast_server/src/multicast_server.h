/*
* File:   multicast_server.h
* Class:  CMPT 332 Assignment 4
* Author: Kyle and Taran
* Date:   December 03 2015
*
* Description: Header file for the multicast server implementation
*/
#ifndef MULTICAST_SERVER_H
#define MULTICAST_SERVER_H

#define BACKLOG 10 /* Number of pending connections for listen queue */
#define RECV_CLIENT_PORT 34000
#define SEND_CLIENT_PORT 31000

/* Static and local shared variables ----------------------------------------*/
long send_client_port;
long recv_client_port;

pthread_mutex_t q_lock;     /* Lock for queue and shared vars */
pthread_cond_t  new_msg;    /* CV broadcasted on when new msg arrives */

volatile int num_recv_clients;  /* Number of connected recv clients */
volatile int num_msgs;          /* Number of messages currently in queue */
volatile int seq_num;           /* Current queue node sequence number */

static volatile GQueue *msg_q;     /* Global message queue items of type Msg */

typedef struct {
	char* msg;  /* Stored message from send client */
	int   seq;  /* Sequence number for this message */
    int   numC; /* Number of clients to process this message */
} Msg;

/* Function prototypes ------------------------------------------------------*/

/* Description: Server thread that listens on a socket
*  Inputs: p - the portnumber to listen on
*
*  Return: -1 On Failure, 0 on Success
*/
void *listener_thread(void *prt);

/* Description: Server thread that handles a send client
*  Inputs: sock_fd - connected socket to client
*
*  Return: none
*/
void *send_client_thread(void *sock_fd);

/* Description: Server thread that handles a recv client
*  Inputs: sock_fd - connected socket to client
*
*  Return: none
*/
void *recv_client_thread(void *sock_fd);

#endif