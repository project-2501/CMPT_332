/*
* File:   test_dogwash.c
* Class:  CMPT 332 Assignment 3
* Author: Kyle and Taran
* Date:   November 03 2015
*
* Description: Test program for the dogwash synchronization problem. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "dogwashsynch.h"

static pthread_t *create_dogs(int num_DA, int num_DB, int num_DO);

int main(int argc, char *argv[]) {

	int num_bays = 0;
	pthread_t *dogs = NULL;  /* Pointer to array of dog pthreads */

	/* Take one command line arguments ***************************************/
	if (argc < 2) {
		fprintf(stderr, "Usage: ./test_dogwash num_bays \n");
		return EXIT_FAILURE;
	}
	else {
		num_bays = atoi(argv[1]);
	}

	/* Initialize the dogwash *************************************************/
	if (dogwash_init(num_bays) == -1) {
		fprintf(stderr, "Error: could not initialize dog wash\n");
		return EXIT_FAILURE;
	}
	else 
		printf("Initialized DogWash - Bays: %d\n", num_bays);

	/* Create some 'dog' threads **********************************************/
	int num_DA = 1;
	int num_DB = 1;
	int num_DO = 1;
	int total  = num_DA + num_DB + num_DO;

	dogs = create_dogs(num_DA, num_DB, num_DO);
	if (dogs == NULL) {
		fprintf(stderr, "Error: could not create the dogs\n");
		return EXIT_FAILURE;
	}

	for (int i = 0; i < total; i++) {
		if(pthread_join(dogs[i], NULL) != 0) {
			fprintf(stderr, "Test Error: could not join thread\n");
			return EXIT_FAILURE;
		}
	}
	
	printf("%d dog threads exited sucessfully\n", total);

	/* Tear down the system ***************************************************/
	if (dogwash_done() == -1) {
		fprintf(stderr, "Error: could not shutdown dog wash\n");
		return EXIT_FAILURE;
	}
	else
		printf("Shutdown DogWash\n");

	/* Free memory pointed to by dogs */
	free(dogs);

	return EXIT_SUCCESS;
}

/* Description: Creates a range of 'dog' pthreads. Can pass in 
*               a varying amount of different dog types to compete for a 
*               bay in the dogwash. 
*  Inputs:
*   int num_DA - the number of DA dogs to create
*   int num_DB - the number of DB dogs to create
*   int num_DO - the number of DO dogs to create
*
*  Return:
*   pthread_t *dogs - Success: pointer to array of pthreads (dogs)
*                     Failure: NULL
*
*   USAGE WARNING: Caller must free() array pointer returned!
*/
static pthread_t * 
create_dogs(int num_DA, int num_DB, int num_DO) {

	int total = num_DA + num_DB + num_DO;

	/* Allocate storage for number of threads */
	pthread_t *dogs = malloc(total * sizeof(pthread_t));
	if (dogs == NULL) 
		return NULL;

	/* Create the DA dogs */
	for (int i =0; i < num_DA; i++) {
		if(pthread_create(&dogs[i], NULL, (void *) (newdog), (void *) DA) != 0){
			/* Error Occurred */
			free(dogs);
			return NULL;
		}
	}

	/* Create the DB dogs */
	for (int i = num_DA; i < (num_DA + num_DB); i++) {
		if(pthread_create(&dogs[i], NULL, (void *) newdog, (void *) DB) != 0){
			/* Error Occurred */
			free(dogs);
			return NULL;
		}
	}

	/* Create the DC dogs */
	for (int i = (num_DA + num_DB); i < total; i++) {
		if(pthread_create(&dogs[i], NULL, (void *) newdog, (void *) DO) != 0){
			/* Error Occurred */
			free(dogs);
			return NULL;
		}
	}
	return dogs;
}

