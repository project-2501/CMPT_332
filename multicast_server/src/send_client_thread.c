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

static Msg *make_Msg(char *m);
static void prt(gpointer item);

void *send_client_thread(void *cc) {

	ClientConn cliInfo = *(ClientConn *) cc;
	printf("Send Client Handler: %d\n", cliInfo.fd);
    printf("Send Client IP: %s\n", cliInfo.c_addr);
    printf("Send Client Port: %d\n", cliInfo.c_port);
    int sock = cliInfo.fd;

	// read text from socket into local buffer
    char buf[MAXDATASIZE];   // hold received message
    char msg[MAXDATASIZE + 64];   // buf prefixed by info
    int nbytes;

    for(;;) {

        // receive message
        if ((nbytes = recv(sock, buf, sizeof buf, 0)) <= 0) {
            // got error or connection closed by client
            if (nbytes == 0) {
                // connection closed
                printf("server: socket %d hung up\n", sock);
            } else {
                perror("recv");
            }
            close(sock);
            pthread_exit(NULL);
        }
        buf[nbytes] = '\0';
        printf("server received: %s", buf);
        sprintf(msg, "%s, %d: %s", cliInfo.c_addr, cliInfo.c_port, buf);
        
        // aquire message queue lock
        if (pthread_mutex_lock(&q_lock) != 0) {
            perror("pthread_mutex_lock");
            pthread_exit(NULL);
        }

        // discard message if no recv clients
        if(num_recv_clients == 0) {
            if (pthread_mutex_unlock(&q_lock) != 0) {
                perror("pthread_mutex_unlock");
                pthread_exit(NULL);
            }           
            continue;
        }
        else {
            // add msg to the msg_queue
            seq_num++;
            num_msgs++;
            g_queue_push_tail(msg_q, make_Msg(msg));
            // broadcast on new_msg condition variable
            if (pthread_cond_broadcast(&new_msg) != 0) {
                perror("pthread_cond_broadcast");
                pthread_exit(NULL);
            }
            printf("queue is currently:\n");
            g_queue_foreach(msg_q, (GFunc)prt, NULL);
        }

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
	message->msg = strndup(m, MAXDATASIZE + 64);
	message->seq  = seq_num;
	message->numC = num_recv_clients;
	return message;
}

static void
prt(gpointer item) {
    printf("%s %d %d\n", ((Msg*)item)->msg, ((Msg*)item)->seq, ((Msg*)item)->numC);
}
