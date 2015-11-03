/*
* File:   dogwashsynch.c
* Class:  CMPT 332 Assignment 3
* Author: Kyle and Taran
* Date:   November 03 2015
*
* Description: Implementation of the dogwash synchronization interface 
*              problem interface. This version is implemented using
*              posix semaphores.
*/

int dogwash_init(int numbays);
int newdog(dogtype dog);
int dogdone(dogtype dog);
int dogwash_done(void);
