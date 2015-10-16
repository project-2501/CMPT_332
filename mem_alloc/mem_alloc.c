/*
 * File:   mem_alloc.c
 * Class:  CMPT 332 Assignment 2
 * Author: Kyle and Taran
 * Date:   October 12 2015
 *
 * Description: Implementation of memory allocator with the following:
 *               - doubly linked free list approach
 *               - each chunk contains a header and footer
 *               - next fit policy for free list
 *               - chunk size rounded to nearest multiple of 16
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "mem_alloc.h"

const int MAGIC = 0xDEADBEEF;
const int NODE_SIZE  = sizeof(h_node_t) + sizeof(f_node_t);
const int BLOCK_SIZE = sizeof(header_t) + sizeof(footer_t);

/* Pointer to the head of the free list */
static h_node_t *head = NULL;

/* Pointer to location of last allocation into free list */
static h_node_t *next = NULL;

/* Function implementations **************************************************/
int M_Init(int size) {

	/* Test that this function is not called twice */
	if (head != NULL)
		return -1;

	void *addr;
	f_node_t *footer;

	/* Calculate total size for new mapping */
	int	total_size = size + NODE_SIZE;

	/* Create an anonymous private mapping */
	addr = mmap(NULL, total_size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	if (addr == MAP_FAILED)
		return -1;

	printf("M_Init(): Size %d at address %p\n\n", total_size, addr);

	/* Setup the head of the free list */
	head = (h_node_t *) addr;
	head->size = size;
	head->next = NULL;
	next = head;

	/* Place the footer at the end of the memory region */
	footer = (f_node_t *) (addr + size + sizeof(h_node_t));
	footer->size = size;
	footer->prev = NULL;

	return 0;
}

void *M_Alloc(int size) {

	if (next == NULL)
		next = head;

	/* Calculate total size of request */
	int	total_size = size + BLOCK_SIZE;
	header_t *b_head;
	footer_t *b_foot;
	f_node_t *n_foot;
	void *new_addr = NULL;

	/* Apply next fit to find next available chunk */
	do
	{
		if (total_size <= next->size) {

			int temp_size = next->size;
			h_node_t *temp_n_head = next->next;

			/* Stamp header and footer onto new memory block */
			b_head = (header_t *) next;
			b_head->size = size;
			b_head->magic = MAGIC;
			b_foot = (footer_t *) ( ((void *) b_head) + b_head->size + sizeof(header_t));
			b_foot->size = size;
			b_foot->magic = MAGIC;
			new_addr = (void *) ( ((void *) b_head) + sizeof(header_t));

			/* Update this block of the free list */
			next = (h_node_t *) ( ((void *) b_head) + b_head->size + BLOCK_SIZE);
			next->size = temp_size - total_size;
			next->next = temp_n_head;
			n_foot = (f_node_t *) ( ((void *) next) + next->size + sizeof(h_node_t));
			n_foot->size = temp_size - total_size;
			if ( (void *) head == (void *) b_head)
				head = next;

			break;
		}
		else {
			/* Check next node on free list */
			next = next->next;
		}

	} while(next != NULL);
	
	/* Return NULL if no space found */
	if (next == NULL)
		return NULL;
	else
		return new_addr;
}

int M_Free(void *p) {

	/* Verify this is a valid pointer */
	footer_t *prev_foot = (footer_t *) (p - BLOCK_SIZE);
	if (prev_foot->magic != MAGIC)
		return -1;

	/* Add this chunk to the free list */

	return 0;
}

void M_Display() {
	
	int count = 0;
	h_node_t *itr = head;
	f_node_t *footer;

	/* Iterate through free list and display block information */
	do
	{
		/* Print the header */
		printf("Summary Block %d:\n", count);
		printf("  Header\n");
		printf("    Size %d\n", itr->size);
		printf("    Next %p\n", itr->next);

		/* Print the footer */
		footer = (f_node_t *) ( ((void *) itr) + itr->size + sizeof(h_node_t));
		printf("  Footer:\n");
		printf("    Size %d\n", footer->size);
		printf("    Prev %p\n", footer->prev);
		itr = itr->next;
	} while (itr != NULL);
}
