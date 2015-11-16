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
#include <semaphore.h>
#include <unistd.h>
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


static volatile int A_counter;
static volatile int B_counter;

static sem_t empty;
static sem_t dogASwitch;
static sem_t dogBSwitch;
static sem_t dogAMultiplex;
static sem_t dogBMultiplex;
static sem_t turnstile;

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

    A_counter = 0;
    B_counter = 0;

	/* (Re)Initialize NOTE: have to do this dynamically */
	if (sem_init(&empty, 0, 1) != 0) {
        printf("%lu - sem_init(&m) failure\n", pthread_self());
		return EXIT_FAILURE;
    }

	if (sem_init(&dogASwitch, 0, 1) != 0) {
        printf("%lu - sem_init(&c) failure\n", pthread_self());
		return EXIT_FAILURE;
    }

	if (sem_init(&dogBSwitch, 0, 1) != 0) {
        printf("%lu - sem_init(&m) failure\n", pthread_self());
		return EXIT_FAILURE;
    }

	if (sem_init(&dogAMultiplex, 0, numbays) != 0) {
        printf("%lu - sem_init(&m) failure\n", pthread_self());
		return EXIT_FAILURE;
    }

	if (sem_init(&dogBMultiplex, 0, numbays) != 0) {
        printf("%lu - sem_init(&m) failure\n", pthread_self());
		return EXIT_FAILURE;
    }

	if (sem_init(&turnstile, 0, 1) != 0) {
        printf("%lu - sem_init(&m) failure\n", pthread_self());
		return EXIT_FAILURE;
    }

	return EXIT_SUCCESS;
}

int newdog(dogtype my_type){

	printf("%lu - dog of type %d waiting\n", pthread_self(), my_type);

	if (my_type == DA) {

        if (sem_wait(&turnstile) != 0) {
            return EXIT_FAILURE;
        }

        if (sem_wait(&dogASwitch) != 0) {
            return EXIT_FAILURE;
        }
        A_counter += 1;
        if (A_counter == 1) {
            if (sem_wait(&empty) != 0) {
                return EXIT_FAILURE;
            }
        }
        if (sem_post(&dogASwitch) != 0) {
            return EXIT_FAILURE;
        }

        if (sem_post(&turnstile) != 0) {
            return EXIT_FAILURE;
        }

        if (sem_wait(&dogAMultiplex) != 0) {
            return EXIT_FAILURE;
        }
        // dog wash code

	}
	else if (my_type == DB) {

        if (sem_wait(&turnstile) != 0) {
            return EXIT_FAILURE;
        }

        if (sem_wait(&dogBSwitch) != 0) {
            return EXIT_FAILURE;
        }
        B_counter += 1;
        if (B_counter == 1) {
            if (sem_wait(&empty) != 0) {
                return EXIT_FAILURE;
            }
        }
        if (sem_post(&dogBSwitch) != 0) {
            return EXIT_FAILURE;
        }

        if (sem_post(&turnstile) != 0) {
            return EXIT_FAILURE;
        }

        if (sem_wait(&dogBMultiplex) != 0) {
            return EXIT_FAILURE;
        }

	}
	else { /* my_type == DO */

        

	}

    printf("%lu - dog of type %d entered bay\n", pthread_self(), my_type);
    // wait for some time while doing wash
    usleep(5000);
	dogdone(my_type);
    return EXIT_SUCCESS;
}

int dogdone(dogtype my_type) {

	/* Assign type to this dog thread */
	printf("%lu - dog of type %d is done\n", pthread_self(), my_type);

	if (my_type == DA) {

        if (sem_post(&dogAMultiplex) != 0) {
            return EXIT_FAILURE;
        }
        if (sem_wait(&dogASwitch) != 0) {
            return EXIT_FAILURE;
        }
        A_counter -= 1;
        if (A_counter == 0) {
            if (sem_post(&empty) != 0) {
                return EXIT_FAILURE;
            }
        }
        if (sem_post(&dogASwitch) != 0) {
            return EXIT_FAILURE;
        }

	}
	else if (my_type == DB) {

        if (sem_post(&dogBMultiplex) != 0) {
            return EXIT_FAILURE;
        }
        if (sem_wait(&dogBSwitch) != 0) {
            return EXIT_FAILURE;
        }
        B_counter -= 1;
        if (B_counter == 0) {
            if (sem_post(&empty) != 0) {
                return EXIT_FAILURE;
            }
        }
        if (sem_post(&dogBSwitch) != 0) {
            return EXIT_FAILURE;
        }

	}
	else { /* my_type == DO */



	}

	return EXIT_SUCCESS;
}

int dogwash_done(void) {

	/* Destroy the current mutex and condition variables */
	if (sem_destroy(&empty) != 0) {
        printf("%lu - sem_destroy(&m) failure\n", pthread_self());
		return EXIT_FAILURE;
    }

	if (sem_destroy(&dogASwitch) != 0) {
        printf("%lu - sem_destroy(&m) failure\n", pthread_self());
		return EXIT_FAILURE;
    }
	
    if (sem_destroy(&dogBSwitch) != 0) {
        printf("%lu - sem_destroy(&m) failure\n", pthread_self());
		return EXIT_FAILURE;
    }
	
    if (sem_destroy(&dogAMultiplex) != 0) {
        printf("%lu - sem_destroy(&m) failure\n", pthread_self());
		return EXIT_FAILURE;
    }
	
    if (sem_destroy(&dogBMultiplex) != 0) {
        printf("%lu - sem_destroy(&m) failure\n", pthread_self());
		return EXIT_FAILURE;
    }
	
    if (sem_destroy(&turnstile) != 0) {
        printf("%lu - sem_destroy(&m) failure\n", pthread_self());
		return EXIT_FAILURE;
    }
	
    return 0;
}
