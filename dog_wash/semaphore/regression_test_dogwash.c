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
static int unit_test_alt(int num_bays, int num_each);
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
    printf("------------\n");
    printf("TEST 1 BEGIN\n");
	if (unit_test_N(1,3,0,0) == EXIT_FAILURE) {
		fprintf(stderr, "Unit Test %d failed\n", 1);
		result++;
	}
    printf("TEST 1 END\n");
    printf("------------\n");

	/* 1 DA and 1 DB competing for 2 bays */
    printf("------------\n");
	printf("TEST 2 BEGIN\n");
    if (unit_test_N(2,1,1,0) == EXIT_FAILURE) {
		fprintf(stderr, "Unit Test %d failed\n", 2);
	    result++;
	}
    printf("TEST 2 END\n");
    printf("------------\n");

	/* 5 DA's and 5 DB's competing for 2 bays */
    printf("------------\n");
	printf("TEST 3 BEGIN\n");
    if (unit_test_N(2,5,5,0) == EXIT_FAILURE) {
		fprintf(stderr, "Unit Test %d failed\n", 3);
	    result++;
	}
    printf("TEST 3 END\n");
    printf("------------\n");

    /* DA's and DB's entering in alternating order */
    printf("------------\n");
    printf("TEST 4 BEGIN\n");
    if (unit_test_alt(10, 20) == EXIT_FAILURE) {
        fprintf(stderr, "Unit Test %d failed\n", 4);
        result++;
    }
    printf("TEST 4 END\n");
    printf("------------\n");

    /* Large amound of dogs */
    /*
    printf("------------\n");
    printf("TEST 5 BEGIN\n");
    if (unit_test_N(5, 100, 100, 100) == EXIT_FAILURE) {
        fprintf(stderr, "Unit Test %d failed\n", 5);
        result++;
    }
    printf("TEST 5 END\n");
    printf("------------\n");
    */

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
    else {
        printf("Initialized DogWash - Bays: %d\n", num_bays);
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
    printf("%d dog threads exited successfully\n", total);

	/* Clean up the simulation */
	if (dogwash_done() == -1) {
		fprintf(stderr, "Error - UT1: could not shutdown dog wash\n");
		return EXIT_FAILURE;
	}
    else
        printf("Shutdown DogWash\n");

	/* Free the memory allocated by create_dogs() */
	free(dogs);
	return EXIT_SUCCESS;
}

/* Description: Test utilization of the dogwash by changin the order
 * in which dogs arrive and their time of arrival.*/
static int unit_test_alt(int num_bays, int num_each) {
    
    int total = 2 * num_each;

    if (dogwash_init(num_bays) == -1) {
        fprintf(stderr, "Error - UT3: could not initialize dog wash\n");
        return EXIT_FAILURE;
    }
    else {
        printf("Initialized DogWash - Bays: %d\n", num_bays);
    }

    /* Create num_each of A and B dogs */
	/* Allocate storage for number of threads */
	pthread_t *dogs = malloc(total * sizeof(pthread_t));
	if (dogs == NULL) {
        fprintf(stderr, "Error - UT3: could not create dogs\n"); 
		return EXIT_FAILURE;
    }

	/* Create the DA and DB dogs, alternating get a good mix */
	for (int i =0; i < total; i++) {
        if(i % 2 == 0) {
		    if(pthread_create(&dogs[i], NULL, (void *) (dog), (void *) DA) != 0){
			    /* Error Occurred */
			    free(dogs);
                fprintf(stderr, "Error - UT3: could not create dogs\n"); 
		        return EXIT_FAILURE;
		    }
        }
        else {
		    if(pthread_create(&dogs[i], NULL, (void *) dog, (void *) DB) != 0){
			    /* Error Occurred */
			    free(dogs);
                fprintf(stderr, "Error - UT3: could not create dogs\n"); 
		        return EXIT_FAILURE;
		    }
        }
        usleep(100000);
	}

    /* Join the dog threads as they complete */
    for (int i = 0; i < total; i++) {
        if(pthread_join(dogs[i], NULL) != 0) {
            fprintf(stderr, "Test Error: could not join thread\n");
            return EXIT_FAILURE;
        }
    }
    printf("%d dog threads exited successfully\n", total);

    /* Clean up the simulation */
    if (dogwash_done() != 0) {
        fprintf(stderr, "Error - UT3: could not shutdown dog wash\n");
        return EXIT_FAILURE;
    }
    else
        printf("Shutdown DogWash\n");

    /* Free the memory allocated by creating dogs */
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

/* Description: Sends a dog of type mytype through wash bay system by calling
 * newdog, then waiting to simulate wash time, then calling dogdone. */
static int dog(dogtype mytype) {
    printf("%lu - dog of type %d waiting for bay\n", pthread_self(), mytype);
    if (newdog(mytype) != 0 ) {
        return EXIT_FAILURE;
    }
    printf("%lu - dog of type %d entering bay\n", pthread_self(), mytype);
    // simulate time spent washing dog
    usleep(500000);
    printf("%lu - dog of type %d is done washing\n", pthread_self(), mytype);
    if (dogdone(mytype) != 0) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
