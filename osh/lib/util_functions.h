/*
 * Description: Header file for collection of utility functions
 *
 * Authors: Taran and Kyle
 * Class:   CMPT 332 - Fall 2015
 *
 */

#ifndef UTIL_FUNCTIONS_H
#define UTIL_FUNCTIONS_H

void reverse_array(char *str[], int n);

char *trimwhitespace(char *str);

void compress_spaces(char *str);

int split(char *string, char *delim, char *tokens[]);

int num_chars(const char *str, const char c);

#endif
