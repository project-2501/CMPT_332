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
*              implementation tweaks don't break old tests.
*
* Usage: To add a new test, do the following:
* 		 
* 		 -> Add a new function declaration (eg. static int unit_test_n(void))
* 		 -> Copy-Paste a previous test     (eg. unit_test_1)
* 		 -> Adjust the parameters to suit  (eg. num_bays, num_DA, etc.)
* 		 -> Document your test scenario    (eg. 5 DA's and 5 DB's) 
* 		 -> Add code to run_unit_tests to invoke your new unit test
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "dogwashsynch.h"

/* Local function declarations */
static pthread_t *create_dogs(int num_DA, int num_DB, int num_DO);
static int run_unit_tests(void);
static int unit_test_1(void);
static int unit_test_2(void);

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

	if (unit_test_1() == EXIT_FAILURE) {
		fprintf(stderr, "Unit Test 1 failed\n");
		result++;
	}
	if (unit_test_2() == EXIT_FAILURE) {
		fprintf(stderr, "Unit Test 2 failed\n");
		result++;
	}

	return result;
}

/*
 * Test Scenario:
 *  Tests creation of multiple DA's synchronizing access to single bay
 */
static int 
unit_test_1(void) {

	/* System parameters for this unit test */
	int num_bays = 1;
	int num_DA = 3;
	int num_DB = 0;
	int num_DO = 0;
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

	for (int i = 0; i < total; i++) {
		if(pthread_join(dogs[i], NULL) != 0) {
			fprintf(stderr, "Test Error: could not join thread\n");
			return EXIT_FAILURE;
		}
	}

	if (dogwash_done() == -1) {
		fprintf(stderr, "Error - UT1: could not shutdown dog wash\n");
		return EXIT_FAILURE;
	}

	free(dogs);
	return EXIT_SUCCESS;
}

/*
 * Test Scenario:
 *  Tests creation of multiple DA's and DB's
 */
static int 
unit_test_2(void) {

	/* System parameters for this unit test */
	int num_bays = 3;
	int num_DA = 3;
	int num_DB = 3;
	int num_DO = 0;
	int total  = num_DA + num_DB + num_DO;
	pthread_t *dogs;

	if (dogwash_init(num_bays) == -1) {
		fprintf(stderr, "Error - UT2: could not initialize dog wash\n");
		return EXIT_FAILURE;
	}

	/* Create a variety of dogs here */
	dogs = create_dogs(num_DA, num_DB, num_DO);
	if (dogs == NULL) { 
		fprintf(stderr, "Error - UT2: could not create dogs\n");
		return EXIT_FAILURE;
	}

	for (int i = 0; i < total; i++) {
		if(pthread_join(dogs[i], NULL) != 0) {
			fprintf(stderr, "Test Error: could not join thread\n");
			return EXIT_FAILURE;
		}
	}

	if (dogwash_done() == -1) {
		fprintf(stderr, "Error - UT2: could not shutdown dog wash\n");
		return EXIT_FAILURE;
	}

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
