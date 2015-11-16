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

static sem_t noAorB;            /* 1 if no DAs or DBs in wash, 0 otherwise */
static sem_t dogASwitch;        /* Binary sem for modifying A state variables*/ 
static sem_t dogBSwitch;        /* Binary sem for modifying B state variables*/
static sem_t baysAvail;         /* > 0 if there is a bay available */
static sem_t turnstile;         /* Prevent starvation of A or B dogs */

static sem_t state_mutex;
static int turnstileActive;

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
    turnstileActive = 0;

	/* (Re)Initialize NOTE: have to do this dynamically */
	if (sem_init(&noAorB, 0, 1) != 0) {
		return -1;
    }

	if (sem_init(&dogASwitch, 0, 1) != 0) {
		return -1;
    }

	if (sem_init(&dogBSwitch, 0, 1) != 0) {
		return -1;
    }

	if (sem_init(&baysAvail, 0, numbays) != 0) {
		return -1;
    }

	if (sem_init(&turnstile, 0, 1) != 0) {
		return -1;
    }

    if (sem_init(&state_mutex, 0, 1) != 0) {
        return -1;
    }

	return 0;
}

int newdog(dogtype my_type){

	if (my_type == DA) {

        int forced = 0;

        if (turnstileActive == 1){
        if (sem_wait(&turnstile) != 0) {
            return -1;
        }
        forced = 1;
        }

        if (sem_wait(&dogASwitch) != 0) {
            return -1;
        }
        A_washing += 1;
        if (A_washing == 1) {
            if (sem_wait(&noAorB) != 0) {
                return -1;
            }
        }

        if (sem_post(&dogASwitch) != 0) {
            return -1;
        }

        if (forced == 1) {
        if (sem_post(&turnstile) != 0) {
            return -1;
        }
        forced = 0;
        }

        if (sem_wait(&baysAvail) != 0) {
            return -1;
        }

	}
	else if (my_type == DB) {

        int forced = 0;

        if (turnstileActive == 1) {
        if (sem_wait(&turnstile) != 0) {
            return -1;
        }
        forced = 1;
        }

        if (sem_wait(&dogBSwitch) != 0) {
            return -1;
        }
        B_washing += 1;
        if (B_washing == 1) {
            if (sem_wait(&noAorB) != 0) {
                return -1;
            }
        }
        if (sem_post(&dogBSwitch) != 0) {
            return -1;
        }

        if (forced == 1) {
        if (sem_post(&turnstile) != 0) {
            return -1;
        }
        forced = 0;
        }

        if (sem_wait(&baysAvail) != 0) {
            return -1;
        }

	}
	else { /* my_type == DO */

        if (sem_wait(&baysAvail) != 0) {
            return -1;
        }        

	}

    return 0;
}

int dogdone(dogtype my_type) {

	/* Assign type to this dog thread */

	if (my_type == DA) {

        if (sem_post(&baysAvail) != 0) {
            return -1;
        }
        if (sem_wait(&dogASwitch) != 0) {
            return -1;
        }
        if (sem_wait(&state_mutex) != 0) {
            return -1;
        }
        A_washing -= 1;
        A_count += 1;
        if (A_count >= thresh) {
            turnstileActive = 1;
            printf("A turnstile on\n");
        }
        if (B_count >= thresh) {
            turnstileActive = 0;
            printf("A turnstile off\n");
            B_count = 0;
        }
        if (sem_post(&state_mutex) != 0) {
            return -1;
        }
        if (A_washing == 0) {
            if (sem_post(&noAorB) != 0) {
                return -1;
            }
        }
        if (sem_post(&dogASwitch) != 0) {
            return -1;
        }

	}
	else if (my_type == DB) {

        if (sem_post(&baysAvail) != 0) {
            return -1;
        }
        if (sem_wait(&dogBSwitch) != 0) {
            return -1;
        }
        if (sem_wait(&state_mutex) != 0) {
            return -1;
        }
        B_washing -= 1;
        B_count += 1;
        if (B_count >= thresh) {
            turnstileActive = 1;
            printf("B turnstile on\n");
        }
        if (A_count >= thresh) {
            turnstileActive = 0;
            printf("B turnstile off\n");
            A_count = 0;
        }
        if (sem_post(&state_mutex) != 0) {
            return -1;
        }
        if (B_washing == 0) {
            if (sem_post(&noAorB) != 0) {
                return -1;
            }
        }
        if (sem_post(&dogBSwitch) != 0) {
            return -1;
        }

	}
	else { /* my_type == DO */

        if (sem_post(&baysAvail) != 0) {
            return -1;
        }

	}

	return 0;
}

int dogwash_done(void) {

	/* Destroy the current mutex and condition variables */
	if (sem_destroy(&noAorB) != 0) {
		return -1;
    }

	if (sem_destroy(&dogASwitch) != 0) {
		return -1;
    }
	
    if (sem_destroy(&dogBSwitch) != 0) {
		return -1;
    }
	
    if (sem_destroy(&baysAvail) != 0) {
		return -1;
    }
	
    if (sem_destroy(&turnstile) != 0) {
		return -1;
    }
	
    if (sem_destroy(&state_mutex) != 0) {
        return -1;
    }

    return 0;
}
