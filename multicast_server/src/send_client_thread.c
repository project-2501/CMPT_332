/*
* File:   send_client_thread.c
* Class:  CMPT 332 Assignment 4
* Author: Kyle and Taran
* Date:   December 03 2015
*
* Description: Listener thread for the server to accept new connections
*/
#include <stdio.h>
#include <pthread.h>
#include <glib.h>

#include "multicast_server.h"

void *send_client_thread(void *sock_fd) {

	int sock = *(int *) sock_fd;
	printf("Send Client Handler: %d\n", sock);
	// read text from socket into local buffer
    // Implement logic from paper design...                              TODO

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
	message->msg  = m;
	message->seq  = seq_num;
	message->numC = num_recv_clients;
	return message;
}