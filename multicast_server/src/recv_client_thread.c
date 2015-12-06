/*
* File:   recv_client_thread.c
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

void *recv_client_thread(void *sock_fd) {

	int sock = *(int *) sock_fd;
	printf("Recv Client Handler: %d\n", sock);
	// Register into system
    // Implement logic from paper design...                              TODO

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