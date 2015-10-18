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

const long long int MAGIC = 0xDEADBEEFDEADBEEF;
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
	header_t *new_header;
	footer_t *new_footer;
	f_node_t *old_f_node;
	h_node_t *old_next_node;
	h_node_t *old_prev_node;
	void *base_addr = (void *) next;
	int old_size;

	/* Apply next fit to find next available chunk */
	do
	{
		if (size <= next->size) {

			/* Save important information needed shortly */
			old_f_node = (f_node_t *) (base_addr + next->size + sizeof(h_node_t));
			old_next_node = next->next;
			old_prev_node = old_f_node->prev;
			old_size = next->size;

			if ( (old_size - size) == 0) { /* Perfect size match */

				/* Update the next pointer */
				next = old_next_node;

				/* Update the next node not to point back anymore */
				f_node_t *old_next_f_node = (f_node_t *) ((void *)old_next_node + old_next_node->size + sizeof(h_node_t));
				old_next_f_node->prev = old_prev_node;

				/* Stamp header and footer onto new memory block */
				new_header = (header_t *) base_addr;
				new_header->size = size;
				new_header->magic = MAGIC;
				new_footer = (footer_t *) (base_addr + new_header->size + sizeof(header_t));
				new_footer->size = size;
				new_footer->magic = MAGIC;
			}
			else { /* Split into two pieces */
				/* Stamp header and footer onto new memory block */
				new_header = (header_t *) base_addr;
				new_header->size = size;
				new_header->magic = MAGIC;
				new_footer = (footer_t *) (base_addr + new_header->size + sizeof(header_t));
				new_footer->size = size;
				new_footer->magic = MAGIC;

				/* Update this node on the free list */
				next = (h_node_t *) (base_addr + new_header->size + BLOCK_SIZE);
				next->size = old_size - (size + BLOCK_SIZE);
				next->next = old_next_node;
				old_f_node->size = old_size - (size + BLOCK_SIZE);

				/* Update previous free list node to point to this new one */
				if (old_prev_node != NULL)
					old_prev_node->next = next;
			}

			/* Update head pointer if it was previously pointing here */
			if ( (void *) head == (void *) new_header)
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
	else {
		/* Return address just after the header metadata */
		return ((void *) new_header + sizeof(header_t));
	}
}

int M_Free(void *p) {

	/* Verify this is a valid pointer */
	header_t *header = (header_t *) (p - sizeof(header_t));
	if (header->magic != MAGIC)
		return -1;
	
	assert(header->size >= 16);

	/* Calculate size left for this new node */
	int new_size = (header->size + BLOCK_SIZE) - NODE_SIZE;
	
	/* Setup all of the pointers needed to update free list */
	h_node_t *new_head = (h_node_t *) (p - sizeof(header_t));
	f_node_t *new_foot = (f_node_t *) ((p + header->size + sizeof(footer_t)) - sizeof(f_node_t));
	h_node_t *old_head = head;
	f_node_t *old_foot = (f_node_t *) ((void *) head + sizeof(h_node_t) + head->size);

	/* Stamp metadata on this new chunk */
	new_head->size = new_size;
	new_head->next = old_head;
	new_foot->size = new_size;
	new_foot->prev = NULL;  // prev of head node always null
	head = new_head;

	/* Stamp metadata on the old chunk */
	old_foot->prev = (h_node_t *) new_head;

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
		count++;
	} while (itr != NULL);
}
