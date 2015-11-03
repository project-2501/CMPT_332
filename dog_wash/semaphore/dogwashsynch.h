/*
* File:   dogwashsynch.h
* Class:  CMPT 332 Assignment 3
* Author: Kyle and Taran
* Date:   November 03 2015
*
* Description: Header file for our solution to the dogwash synchronization
*              problems.
*/


#ifndef DOGWASHSYNCH_H
#define DOGWASHSYNCH_H

/* Declare static local variables ********************************************/

/* Define Data Types *********************************************************/

/* The types of dogs in the system */
typedef enum {DA, DB, DO} dogtype;

/* Declare Function Prototypes ***********************************************/

/* Description: Initialize the system variables 
*  Inputs:
*   int numbays - The number of wash bays to create in the system
*
*  Return:
*   0 on success, -1 on failure
*/
int dogwash_init(int numbays);

/* Description: Called by thread (a dog) to request a bay to use. Calling 
*               thread will be blocked until a bay is available and there
*               are no conflicting dogtypes present
*  Inputs:
*   dogtype dog - The dogtype of the calling thread 
*
*  Return:
*   0 on success, -1 on failure
*/
int newdog(dogtype dog);

/* Description: Called by thread when it is done using a wash bay 
*  Inputs:
*   dogtype dog - The dogtype of the calling thread 
*
*  Return:
*   0 on success, -1 on failure
*/
int dogdone(dogtype dog);

/* Description: Perform cleanup operations, reset shared variables
*  Inputs:
*  	(none)
*
*  Return:
*   0 on success, -1 on failure
*/
int dogwash_done(void);

#endif
