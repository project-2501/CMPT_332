/*
 * File:   mem_alloc.h
 * Class:  CMPT 332 Assignment 2
 * Author: Kyle and Taran
 * Date:   October 12 2015
 *
 * Description: Header file for mem_alloc implementation
 *
 */

#ifndef MEM_ALLOC_H
#define MEM_ALLOC_H

/* Declare static local variables ********************************************/ 
extern const long long int MAGIC; 
extern const int NODE_SIZE; 
extern const int BLOCK_SIZE; 

/* Define Data Types *********************************************************/
typedef struct header {
	long long int magic;
	int size;
} header_t;

typedef struct footer {
	int size;
	long long int magic;
} footer_t;

typedef struct h_node {
	int size;
	struct h_node *next;
} h_node_t;

typedef struct f_node {
	int size;
	struct h_node *prev;
} f_node_t;

/* Declare Function Prototypes ***********************************************/

/* Description: Initialize memory region to be used as a heap
 * Inputs:
 *  int size - Size required for the new memory region
 *
 * Return:
 *  0 on success, -1 on failure
 */
int M_Init(int size);

/* Description: Allocates a new chunk of memory
 * Inputs:
 *  int size - Size of memory chunk being requested 
 *
 * Return:
 *  Pointer to start of new chunk on success, NULL on failure
 */
void *M_Alloc(int size);


/* Description: Free's a chunk of allocated memory
 * Inputs:
 *  void *p - Pointer to start of memory region 
 *
 * Return:
 *  0 on success, -1 on failure
 */
int M_Free(void *p);

/* Description: Displays state of the free list
 * Inputs:
 *  none
 *
 * Return:
 *  none
 */
void M_Display(void);

#endif
