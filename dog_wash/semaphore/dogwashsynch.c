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
#include <semaphore.h>
#include <unistd.h>
#include "dogwashsynch.h"

/* Static local variables ****************************************************/
static volatile int A_washing;  /* Number of DA dogs in a wash bay */
static volatile int B_washing;  /* Number of DB dogs in a wash bay */

static sem_t noAorB;            /* 1 if no DAs or DBs in wash, 0 otherwise */
static sem_t dogASwitch;        /* Binary sem for switching to DA's turn */ 
static sem_t dogBSwitch;        /* Binary sem for switching to DB's turn */
static sem_t baysAvail;         /* > 0 if there is a bay available */
static sem_t turnstile;         /* Prevent starvation of A or B dogs */

/* Implementation  ***********************************************************/
int dogwash_init(int numbays) {

	/* Reset the state variables */
    A_washing = 0;
    B_washing = 0;

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

	if (sem_init(&turnstile, 0, numbays) != 0) {
		return -1;
    }

	return 0;
}

int newdog(dogtype my_type){

	if (my_type == DA) {

        if (sem_wait(&turnstile) != 0) {
            return -1;
        }

        if (sem_wait(&dogASwitch) != 0) {
            return -1;
        }
        A_washing += 1;
        if (A_washing == 1) {
            // first A or B into wash
            if (sem_wait(&noAorB) != 0) {
                return -1;
            }
        }

        if (sem_post(&dogASwitch) != 0) {
            return -1;
        }

        // allow another waiting dog past turnstile
        if (sem_post(&turnstile) != 0) {
            return -1;
        }

        // wait for open bay
        if (sem_wait(&baysAvail) != 0) {
            return -1;
        }

	}
	else if (my_type == DB) {

        if (sem_wait(&turnstile) != 0) {
            return -1;
        }

        if (sem_wait(&dogBSwitch) != 0) {
            return -1;
        }
        B_washing += 1;
        if (B_washing == 1) {
            // first A or B into wash
            if (sem_wait(&noAorB) != 0) {
                return -1;
            }
        }
        if (sem_post(&dogBSwitch) != 0) {
            return -1;
        }

        // allow another waiting dog past turnstile
        if (sem_post(&turnstile) != 0) {
            return -1;
        }

        // wait for open bay
        if (sem_wait(&baysAvail) != 0) {
            return -1;
        }

	}
	else { /* my_type == DO */

        // wait for open bay
        if (sem_wait(&baysAvail) != 0) {
            return -1;
        }        

	}

    return 0;
}

int dogdone(dogtype my_type) {

	/* Assign type to this dog thread */

	if (my_type == DA) {

        // indicate open bay
        if (sem_post(&baysAvail) != 0) {
            return -1;
        }
        if (sem_wait(&dogASwitch) != 0) {
            return -1;
        }
        A_washing -= 1;
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

        // indicate open bay
        if (sem_post(&baysAvail) != 0) {
            return -1;
        }
        if (sem_wait(&dogBSwitch) != 0) {
            return -1;
        }
        B_washing -= 1;
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

        // inidicate open bay
        if (sem_post(&baysAvail) != 0) {
            return -1;
        }

	}

	return 0;
}

int dogwash_done(void) {

	/* Destroy the semaphores */
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

    return 0;
}
