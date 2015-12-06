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
static pthread_t *launch_listen_threads(void);
static int  server_initialize(void);

/* Main program -------------------------------------------------------------*/
int main(int argc, char *argv[]) {

	pthread_t *listen_threads;  /* Server threads that accept new clients */

	/* Initialize the server */
	if (server_initialize() != 0) {
		fprintf(stderr, "Could not initialize server\n");
		exit(EXIT_FAILURE);
	}

	/* Launch the two listener threads */
	if ((listen_threads = launch_listen_threads()) == NULL) {
		fprintf(stderr, "Could launch listen threads\n");
		exit(EXIT_FAILURE);
	}

	/* Join with the two listener threads, then exit */
	void *rc;
	for (int i = 0; i < 2; i++) {
		if (pthread_join(listen_threads[i], &rc) != 0) {
			fprintf(stderr, "Could not join thread\n");
			exit(EXIT_FAILURE);
		}
	}

	sleep(1);
	free(listen_threads);
	return EXIT_SUCCESS;
}

/* Local Functions Definitions ----------------------------------------------*/

/* Description: Initializes local variables, mutex/CV, and GQueue
 * Inputs: none
 *
 * Return: -1 On Failure, 0 on Success
 */
static int 
server_initialize() {
	
	/* Initialize the mutex lock */
	if (pthread_mutex_init(&q_lock, NULL) != 0)
		return -1;

	/* Initialize the condition variable */
	if (pthread_cond_init(&new_msg, NULL) != 0)
		return -1;

	/* Initialize the Gqueue */
	if ( (msg_q = g_queue_new()) == NULL) {
		return -1;
	}	

	/* Initialize the shared variables */
	num_recv_clients = 0;
	num_msgs = 0;
	seq_num  = 0;
	send_client_port = SEND_CLIENT_PORT;
	recv_client_port = RECV_CLIENT_PORT;

	return 0;
}

/* Description: Launch the two server listen threads
 * Inputs: none 
 *
 * Return: Pointer to array of two pthreads
 */
static pthread_t *
launch_listen_threads(void) {

	pthread_t *lt = malloc(2 * sizeof(pthread_t));
	if (lt == NULL)
		return NULL;

	/* Create the two listening threads */
	if(pthread_create(&lt[0], NULL, listener_thread, (void *)send_client_port) != 0)
		return NULL;

	if(pthread_create(&lt[1], NULL, listener_thread, (void *)recv_client_port) != 0)
		return NULL;

	return lt;
}