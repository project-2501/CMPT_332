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

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <pthread.h>
#include <glib.h>

/* Static and local shared variables *****************************************/
static const long send_client_port = 31000;
static const long recv_client_port = 34000;

static pthread_mutex_t q_lock;     /* Lock for queue and shared vars */
static pthread_cond_t  new_msg;    /* CV broadcasted on when new msg arrives */

static volatile int num_recv_clients;  /* Number of connected recv clients */
static volatile int num_msgs;          /* Number of messages currently in queue */
static volatile int seq_num;           /* Current queue node sequence number */

static volatile GQueue *msg_q;     /* Global message queue items of type Msg */

typedef struct {
	char* msg;  /* Stored message from send client */
	int   seq;  /* Sequence number for this message */
    int   numC; /* Number of clients to process this message */
} Msg;

/* Local function prototypes *************************************************/
static pthread_t *launch_listen_threads(void); /* Launche the listen threads */
static int  server_initialize(void);           /* Initialize the server */
static Msg *make_Msg(char *m);                 /* Allocates a new Msg */
static void free_Msg(gpointer msg);            /* Free's a Msg */

static void *listener_thread(void *p);    /* Listener thread */
static void *send_client_thread(void *sock_fd); /* Handler for send clients */
static void *recv_client_thread(void *sock_fd); /* Handler for recv clients */

/* Main program **************************************************************/
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
	for (int i = 2; i < 2; i++) {
		if (pthread_join(listen_threads[i], &rc) != 0) {
			fprintf(stderr, "Could not join thread\n");
			exit(EXIT_FAILURE);
		}
	}

	sleep(1);
	free(listen_threads);
	return EXIT_SUCCESS;
}

/* Local Functions Definitions ***********************************************/

/* Description: Server thread that listens on a socket
 * Inputs: p - the portnumber to listen on
 *
 * Return: -1 On Failure, 0 on Success
 */
static void 
*listener_thread(void *p) {

	/* Cast the port number to be used for this thread */
	long port = (long) p;

	printf("Listening on port: %ld\n", (long) p);
	fflush(stdout);


    // Add in server socket code                                        TODO

	/* Branch depending on which port was passed in */
	if (port == send_client_port) {

	}
	else {

	}

	pthread_exit(NULL);
}

/* Description: Server thread that handles a send client
 * Inputs: sock_fd - connected socket to client
 *
 * Return: none
 */
static void 
*send_client_thread(void *sock_fd) {

	// read text from socket into local buffer
    // Implement logic from paper design...                              TODO

	pthread_exit(NULL);
}

/* Description: Server thread that handles a recv client
 * Inputs: sock_fd - connected socket to client
 *
 * Return: none
 */
static void 
*recv_client_thread(void *sock_fd) {

	// Register into system
    // Implement logic from paper design...                              TODO

	pthread_exit(NULL);
}

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

/* Description: Allocate and initialize new Msg struct to store in msg_q
 * Inputs: m - pointer to message string -------------NOTE: have to decide whether this is static or dynamic
 *
 * Return: Pointer to newly allocated Msg struct 
 */
static Msg *
make_Msg(char *m){
	Msg *message = g_new(Msg, 1);
	message->msg  = m;
	message->seq  = seq_num;
	message->numC = num_recv_clients;
	return message;
}

/* Description: Free dynamic memory for Msg stored in in msg_q node
 * Inputs: msg - pointer to the Msg
 *
 * Return: none
 */
static void free_Msg(gpointer msg){
	g_free(msg);
}
