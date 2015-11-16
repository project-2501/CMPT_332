/*
* File:   regression_test_dogwash.c
* Class:  CMPT 332 Assignment 3
* Author: Kyle and Taran
* Date:   November 03 2015
*
* Description: Manages regression tests that were shown to work using
*              test_dogwash.c. Once a scenario is tested and shown to work
*              correctly using test_dogwash.c, add the test scenario in here
*              so that a range of unit tests can be run to make sure that 
*              implementation tweaks don't break old test cases. 
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "dogwashsynch.h"

/* Local function declarations */
static int unit_test_N(int num_bays, int num_DA, int num_DB, int num_DO);
static pthread_t *create_dogs(int num_DA, int num_DB, int num_DO);
static int run_unit_tests(void);
static int dog(dogtype mytype);

/* Main test program **********************************************************/
int main(int argc, char *argv[]) {

	if (run_unit_tests() > 0) {
		fprintf(stderr, "Error: regression test failed\n");
		return EXIT_FAILURE;
	}

	printf("All unit tests completed successfully!\n");
	return EXIT_SUCCESS;
}

/* Local function implementations ********************************************/

/* Description: Run all of the unit tests, report which (if any) failed
*
*/
static int 
run_unit_tests(void) {
	int result = 0;

	/* 3 DA's competing for single bay */
	if (unit_test_N(1,3,0,0) == EXIT_FAILURE) {
		fprintf(stderr, "Unit Test %d failed\n", 1);
		result++;
	}

	/* 2 DA's and 2 DB's competing for 1 bays */
	if (unit_test_N(1,2,2,0) == EXIT_FAILURE) {
		fprintf(stderr, "Unit Test %d failed\n", 2);
		result++;
	}

	return result;
}

/*
 * Descriptions: General test function, pass in system parameters to create
 *               desired unit test.
 */
static int unit_test_N(int num_bays, int num_DA, int num_DB, int num_DO) {

	int total  = num_DA + num_DB + num_DO;
	pthread_t *dogs;

	if (dogwash_init(num_bays) == -1) {
		fprintf(stderr, "Error - UT1: could not initialize dog wash\n");
		return EXIT_FAILURE;
	}

	/* Create a variety of dogs here */
	dogs = create_dogs(num_DA, num_DB, num_DO);
	if (dogs == NULL) { 
		fprintf(stderr, "Error - UT1: could not create dogs\n");
		return EXIT_FAILURE;
	}

	/* Join the dog threads as they complete */
	for (int i = 0; i < total; i++) {
		if(pthread_join(dogs[i], NULL) != 0) {
			fprintf(stderr, "Test Error: could not join thread\n");
			return EXIT_FAILURE;
		}
	}

	/* Clean up the simulation */
	if (dogwash_done() == -1) {
		fprintf(stderr, "Error - UT1: could not shutdown dog wash\n");
		return EXIT_FAILURE;
	}

	/* Free the memory allocated by create_dogs() */
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
		if(pthread_create(&dogs[i], NULL, (void *) (dog), (void *) DA) != 0){
			/* Error Occurred */
			free(dogs);
			return NULL;
		}
	}

	/* Create the DB dogs */
	for (int i = num_DA; i < (num_DA + num_DB); i++) {
		if(pthread_create(&dogs[i], NULL, (void *) dog, (void *) DB) != 0){
			/* Error Occurred */
			free(dogs);
			return NULL;
		}
	}

	/* Create the DC dogs */
	for (int i = (num_DA + num_DB); i < total; i++) {
		if(pthread_create(&dogs[i], NULL, (void *) dog, (void *) DO) != 0){
			/* Error Occurred */
			free(dogs);
			return NULL;
		}
	}
	return dogs;
}

static int dog(dogtype mytype) {
    printf("%lu - dog of type %d waiting for bay\n", pthread_self(), mytype);
    if (newdog(mytype) != 0 ) {
        return EXIT_FAILURE;
    }
    printf("%lu - dog of type %d entering bay\n", pthread_self(), mytype);
    // simulate time spent washing dog
    //sleep(2);
    if (dogdone(mytype) != 0) {
        return EXIT_FAILURE;
    }
    printf("%lu - dog of type %d is done washing\n", pthread_self(), mytype);
    return EXIT_SUCCESS;
}
