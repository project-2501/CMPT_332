/*
* File:   dogwashsynch.c
* Class:  CMPT 332 Assignment 3
* Author: Kyle and Taran
* Date:   November 03 2015
*
* Description: Implementation of the dogwash synchronization interface 
*              problem interface. This version is implemented using
*              locks and condition variables.
*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include "dogwashsynch.h"

/* Static local variables ****************************************************/
static volatile int total_dogs; /* Total number of dogs in the system */
static volatile int num_dogA;   /* Number of DA dogs in the dog wash */
static volatile int num_dogB;   /* Number of DB dogs in the dog wash */
static int num_bays;   /* Number of bays in the system */
static pthread_mutex_t m;
static pthread_cond_t  c;

/* Implementation  ***********************************************************/
int dogwash_init(int numbays) {

	/* Reset the state variables */
	num_bays = numbays;
	num_dogA = 0;
	num_dogB = 0;
	total_dogs = 0;

	/* (Re)Initialize NOTE: have to do this dynamically */
	if (pthread_mutex_init(&m, NULL) != 0)
		return EXIT_FAILURE;

	if (pthread_cond_init(&c, NULL))
		return EXIT_FAILURE;

	return EXIT_SUCCESS;;
}

int newdog(dogtype my_type){

	printf("%lu - dog of type %d started\n", pthread_self(), my_type);

	if (my_type == DA) {

		// CV stuff, add in assertions
		dogdone(my_type);
	}
	else if (my_type == DB) {

		// CV stuff
		dogdone(my_type);
	}
	else { /* my_type == DO */

		// CV stuff
		dogdone(my_type);
	}

	return EXIT_SUCCESS;;
}

int dogdone(dogtype my_type) {

	/* Assign type to this dog thread */
	printf("%lu - dog of type %d is done\n", pthread_self(), my_type);

	if (my_type == DA) {

	}
	else if (my_type == DB) {

	}
	else { /* my_type == DO */

	}

	return EXIT_SUCCESS;
}

int dogwash_done(void) {
	return 0;
}
