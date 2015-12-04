/*
* File:   multicast_server.c
* Class:  CMPT 332 Assignment 4
* Author: Kyle and Taran
* Date:   December 03 2015
*
* Description: Mulithreaded server for relaying messages between
               send_clients and recv_clients
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <glib.h>

/* Static and local shared variables *****************************************/
static pthread_mutex_t q_lock;     /* Lock for queue and shared vars */
static pthread_cond_t  new_msg;    /* CV broadcasted on when new msg arrives */

static volatile num_recv_clients;  /* Number of connected recv clients */
static volatile num_msgs;          /* Number of messages currently in queue */
static seq_num;                    /* Current queue node sequence number */

static volatile GQueue *msg_q;     /* Global message queue items of type Msg */

typedef struct {
	char* msg;  /* Stored message from send client */
	int   seq;  /* Sequence number for this message */
    int   numC; /* Number of clients to process this message */
} Msg;



