#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "mem_alloc.h"

/* Test function prototypes */
int edge_case_unit_test(void);

/* Testing Parameters */
const int MEM_SIZE   = 128;
const int MAX_ALLOCS = 3;
const int ALLOC_SIZE = 16;

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
	
	/* Print out the free list */
	printf("Starting Free List\n");
	M_Display();

	/* Allocate range of chunks */
	for (int i = 0; i < MAX_ALLOCS; i++) {
		if ( (block_ptrs[i] = M_Alloc(ALLOC_SIZE)) == NULL) {
			fprintf(stderr, "M_Alloc failed\n");
			return(EXIT_FAILURE);
		}
	}
	
	/* Print out the free list */
	printf("Free List after %d allocations\n", MAX_ALLOCS);
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
