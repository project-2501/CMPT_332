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
static volatile int bays_in_use;/* Total number of occupied bays */
static volatile int A_waiting;  /* Number of DA dogs waiting for a bay */
static volatile int B_waiting;  /* Number of DB dogs waiting for a bay */
static volatile int A_washing;  /* Number of DA dogs in a wash bay */
static volatile int B_washing;  /* Number of DB dogs in a wash bay */
static volatile int A_count;    /* Fairness variable for DA dogs */
static volatile int B_count;    /* Fairness variable for DB dogs */
static volatile int thresh;     /* When A_count (or B_count) reaches this value
                                   give DB (DA) dogs a chance to get in */
static int num_bays;            /* Number of bays in the system */
static pthread_mutex_t m;
static pthread_cond_t  c;

typedef enum {turn_any, turn_A, turn_B} dog_turn;   /* Which type of dog is  */
static volatile dog_turn turn;                      /* allowed into a bay?   */

/* Implementation  ***********************************************************/
int dogwash_init(int numbays) {

	/* Reset the state variables */
    bays_in_use = 0;
    A_waiting = 0;
    B_waiting = 0;
    A_washing = 0;
    B_washing = 0;
    A_count = 0;
    B_count = 0;
    thresh = numbays;
	num_bays = numbays;
    turn = turn_any;

	/* (Re)Initialize NOTE: have to do this dynamically */
	if (pthread_mutex_init(&m, NULL) != 0)
		return EXIT_FAILURE;

	if (pthread_cond_init(&c, NULL) != 0)
		return EXIT_FAILURE;

	return EXIT_SUCCESS;;
}

int newdog(dogtype my_type){

    if (pthread_mutex_lock(&m) != 0)
        return EXIT_FAILURE;

	printf("%lu - dog of type %d started\n", pthread_self(), my_type);

	if (my_type == DA) {

        if(turn == turn_any)
            turn = turn_A;  // can take a bay immediately without waiting
        A_waiting++;

        while((turn != turn_A) || (bays_in_use == num_bays) || (B_washing > 0))
            if (pthread_cond_wait(&c, &m) != 0)
                return EXIT_FAILURE;
        // secure a bay
        A_waiting--;
        bays_in_use++;
        A_count++;
        A_washing++;

	}
	else if (my_type == DB) {

        if(turn == turn_any)
            turn = turn_B;  // can take a bay immediately without waiting
        B_waiting++;

        while((turn != turn_B) || (bays_in_use == num_bays) || (A_washing > 0))
            if (pthread_cond_wait(&c, &m) != 0)
                return EXIT_FAILURE;
        // secure a bay
        B_waiting--;
        bays_in_use++;
        B_count++;
        B_washing++;

	}
	else { /* my_type == DO */

        while(bays_in_use == num_bays)
            if (pthread_cond_wait(&c, &m) != 0)
                return EXIT_FAILURE;
        // secure a bay
        bays_in_use++;

	}

    if (pthread_mutex_unlock(&m) != 0)
        return EXIT_FAILURE;
    // wait for some time while doing wash
	dogdone(my_type);
    return EXIT_SUCCESS;
}

int dogdone(dogtype my_type) {

    if (pthread_mutex_lock(&m) != 0)
        return EXIT_FAILURE;

	/* Assign type to this dog thread */
	printf("%lu - dog of type %d is done\n", pthread_self(), my_type);

	if (my_type == DA) {

        A_washing--;
        bays_in_use--;

        if(A_count >= thresh) {
            A_count = 0;
            if(B_waiting > 0)
                turn = turn_B;
        }
        else if(A_waiting == 0 && B_waiting > 0)
            turn = turn_B;
        else if(A_waiting == 0 && B_waiting == 0)
            turn = turn_any;

	}
	else if (my_type == DB) {

        B_washing--;
        bays_in_use--;

        if(B_count >= thresh) {
            B_count = 0;
            if(A_waiting > 0)
                turn = turn_A;
        }
        else if(B_waiting == 0 && A_waiting > 0)
            turn = turn_B;
        else if(B_waiting == 0 && A_waiting == 0)
            turn = turn_any;

	}
	else { /* my_type == DO */

        bays_in_use--;

	}

    if (pthread_cond_broadcast(&c) != 0)
        return EXIT_FAILURE;
    if (pthread_mutex_unlock(&m) != 0)
        return EXIT_FAILURE;
	return EXIT_SUCCESS;
}

int dogwash_done(void) {

	/* Destroy the current mutex and condition variables */
	if (pthread_mutex_destroy(&m) != 0)
		return EXIT_FAILURE;

	if (pthread_cond_destroy(&c) != 0)
		return EXIT_FAILURE;
	return 0;
}
