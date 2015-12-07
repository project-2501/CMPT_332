/*
* File:   recv_client_thread.c
* Class:  CMPT 332 Assignment 4
* Author: Kyle and Taran
* Date:   December 03 2015
*
* Description: Listener thread for the server to accept new connections
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <glib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "multicast_server.h"

/* Local function prototypes ------------------------------------------------*/
static void free_Msg(gpointer msg);

void *recv_client_thread(void *sock_fd) {

    /* Cast received argument to integer socket fd */
	int sock = *(int *) sock_fd;

    int local_seq = 0;
    char local_buf[MAXMSGSIZE];
	/* Print status message with socket value */
    printf("Recv Client Handler: %d\n", sock);

    /* Aquire message queue lock */
    if (pthread_mutex_lock(&q_lock) != 0) {
        perror("pthread_mutex_lock");
        pthread_exit(NULL);
    }
    
    /* Increment number of receivers */
    num_recv_clients++;
    
    /* Set local sequence number to one greater than current global sequence
     * number so that only future messages are received by this client */
    local_seq = seq_num + 1;
   
    /* Main loop for sending messages to receiver clients */
    for(;;) {
        /* Wait on condition if there are no messages to send or if sequence
         * indicates that current head of message queue should not be printed */
        while (num_msgs == 0 || g_queue_is_empty(msg_q) ||
            local_seq > ((Msg*)g_queue_peek_head(msg_q))->seq) {
            
            if (pthread_cond_wait(&new_msg, &q_lock) != 0) {
                perror("pthread_cond_wait");
                pthread_exit(NULL);
            }
        }

        if (((Msg*)g_queue_peek_head(msg_q))->seq == local_seq) {
            /* Message should be sent */

            /* Copy message from head of message queue */
            strncpy(local_buf, ((Msg*)g_queue_peek_head(msg_q))->msg,
                MAXMSGSIZE);
            (((Msg*)g_queue_peek_head(msg_q))->numC)--;
            local_seq++;

            /* Remove message from head of queue if this is the last receiver
             * that needs to get it */
            if(((Msg*)g_queue_peek_head(msg_q))->numC == 0) {
                free_Msg(g_queue_pop_head(msg_q));
            }

            /* Release message queue lock while sending message */
            if (pthread_mutex_unlock(&q_lock) != 0) {
                perror("pthread_mutex_unlock");
                pthread_exit(NULL);
            }

            /* Send message */
            if (send(sock, local_buf, sizeof local_buf, 0) == -1) {
                perror("send");

                /* Client has disconnected; update number of clients and exit */
                if (pthread_mutex_lock(&q_lock) != 0) {
                    perror("pthread_mutex_lock");
                    pthread_exit(NULL);
                }
                num_recv_clients--;
                if (pthread_mutex_unlock(&q_lock) != 0) {
                    perror("pthread_mutex_unlock");
                    pthread_exit(NULL);
                }
                pthread_exit(NULL);
            }
           
            /* Reaquire message queue lock before possibly waiting again */
            if (pthread_mutex_lock(&q_lock) != 0) {
                perror("pthread_mutex_lock");
                pthread_exit(NULL);
            }
        }

       
    }

	pthread_exit(NULL);
}

/* Description: Free dynamic memory for Msg stored in in msg_q node
*  Inputs: msg - pointer to the Msg
*
*  Return: none
*/
static void free_Msg(gpointer msg){
	g_free(msg);
}
