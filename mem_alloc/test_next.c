#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "mem_alloc.h"

/* Testing Parameters */
const int MEM_SIZE   = 700;
const int MAX_ALLOCS = 5;
const int ALLOC_SIZE = 64; /* Will be rounded up to 2048 */

int main() {
	int   block_sizes[5] = {256, 112, 64, 32, 16};
	char *block_ptrs[MAX_ALLOCS];

	/* Initialize a memory region */
	if (M_Init(MEM_SIZE) == -1) {
		perror("M_Init failed\n");
		return(EXIT_FAILURE);
	}

	printf("Starting Free List ----------------------\n");
	M_Display();

	/* Allocate range of chunks */
	for (int i = 0; i < MAX_ALLOCS; i++) {
		if ( (block_ptrs[i] = M_Alloc(block_sizes[i])) == NULL) {
			fprintf(stderr, "M_Alloc failed\n");
			return(EXIT_FAILURE);
		}
	}

	printf("\nAfter allocating 5 variable chunks ------\n");
	M_Display();

	printf("\nSelectively release 2 chunks ------------\n");
	if (M_Free(block_ptrs[0]) == -1) {
		fprintf(stderr, "M_Free failed\n");
	    return(EXIT_FAILURE);
	}
	if (M_Free(block_ptrs[2]) == -1) {
		fprintf(stderr, "M_Free failed\n");
	    return(EXIT_FAILURE);
	}
	M_Display();

	printf("\nReallocate chunks of size 80 and 16 ------\n");
	if ( (M_Alloc(80)) == NULL) {
		fprintf(stderr, "M_Alloc failed\n");
		return(EXIT_FAILURE);
	}
	if ( (M_Alloc(16)) == NULL) {
		fprintf(stderr, "M_Alloc failed\n");
		return(EXIT_FAILURE);
	}
	M_Display();

	return 0;
}
