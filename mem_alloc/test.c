#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "mem_alloc.h"

/* Test function prototypes */
int edge_case_unit_test(void);

/* Testing Parameters */
const int MEM_SIZE   = 65536;
const int MAX_ALLOCS = 17;
const int ALLOC_SIZE = 2048;

int main() {
	char *block_ptrs[MAX_ALLOCS];

	/* Initialize a memory region */
	if (M_Init(MEM_SIZE) == -1) {
		perror("M_Init failed\n");
		return(EXIT_FAILURE);
	}

	/* Run the unit tests */
	if (edge_case_unit_test() > 0) {
		fprintf(stderr, "edge_case_unit_test() failed\n");
		return(EXIT_FAILURE);
	}
	
	printf("Starting Free List ----------------------\n");
	M_Display();

	/* Allocate range of chunks */
	for (int i = 0; i < MAX_ALLOCS; i++) {
		if ( (block_ptrs[i] = M_Alloc(ALLOC_SIZE)) == NULL) {
			fprintf(stderr, "M_Alloc failed\n");
			return(EXIT_FAILURE);
		}
	}
	
	printf("\nFree List after %d allocations ----------\n", MAX_ALLOCS);
	M_Display();

	/* Stagger the M_Free's so that it will coalesce 3 different ways */

	printf("\nFreeing everything -----------------------\n");
	for (int i = 0; i < MAX_ALLOCS; i+=4) {
		if (M_Free(block_ptrs[i]) == -1) {
				fprintf(stderr, "M_Free failed\n");
				return(EXIT_FAILURE);
		}
	}

	for (int i = 3; i < MAX_ALLOCS; i+=4) {
		if (M_Free(block_ptrs[i]) == -1) {
				fprintf(stderr, "M_Free failed\n");
				return(EXIT_FAILURE);
		}
	}

	for (int i = 1; i < MAX_ALLOCS; i+=4) {
		if (M_Free(block_ptrs[i]) == -1) {
				fprintf(stderr, "M_Free failed\n");
				return(EXIT_FAILURE);
		}
	}

	for (int i = 2; i < MAX_ALLOCS; i+=4) {
		if (M_Free(block_ptrs[i]) == -1) {
				fprintf(stderr, "M_Free failed\n");
				return(EXIT_FAILURE);
		}
	}

	printf("\nFree List after %d free's ---------------\n", MAX_ALLOCS);
	M_Display();
	
	return 0;
}

/* Try and test all the edge cases */
int edge_case_unit_test(void) {

	int result = 0;
	char *foo;
	/* Try and double initialize */
	if (M_Init(256) == 0) {
		result += 1;
	}

	/* Try and allocate a massive chunk*/
	if ( (foo = M_Alloc(30000000)) != NULL) {
		result += 1;
	}

	/* Try and free a non-valid address */
	if (M_Free(&foo) != -1) {
		result += 1;
	}

	return result;
}
