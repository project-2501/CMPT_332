#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "util_functions.h"

/* Reverses the order of a char * array */
/* NOTE: n is the length of str but has to include +1 for NULL terminator! */
void reverse_array(char *str[], int n)
{
	char *c_str[n];

	int i,j;
	for (i = 0, j = n - 1; j > 0; i++, j--)
		c_str[i] = str[j - 1];

	for (i = 0; i < n-1; i++)
		str[i] = strdup(c_str[i]);

	str[n] = NULL;
	return;
}

/* Trims whitespace off of the front and back of a string */
char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace(*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace(*end)) end--;

  // Write new null terminator
  *(end+1) = 0;
  return str;
}

/* Compresses multiple whitespace to a single space */
void compress_spaces(char *str)
{
    char *dst = str;

    for (; *str; ++str) {
        *dst++ = *str;
        if (isspace(*str)) {
            do 
				++str; 
            while (isspace(*str));
            --str;
        }
    }
    *dst = 0;
	return;
}

/* Splits string by token and return char pointer array 
 * Return: the number of tokens in the string */
int split(char *string, char *delim, char *tokens[])
{
    int count = 0;
    char *token;
    char *stringp;
 
    stringp = string;
    while (stringp != NULL) {
        token = strsep(&stringp, delim);
        tokens[count] = token;
        count++;
    }
    return count;
}

/* Counts number of occurences of char c in str 
 * Return: the character count */
int num_chars(const char *str, const char c)
{
	int count = 0;
	int i = 0;
	for (i = 0; str[i] != '\0'; i++)
	{
		if (str[i] == c)
			count++;
	}
	return count;
}

/* Duplicates every occurrence of char c in str */
void dup_char(char *str, const char c)
{
	int count = num_chars(str, c);
	int origSize = strlen(str);

	char dupStr[origSize + count];
	int size = origSize+count;
	strcpy(dupStr, str);

	int i = 0;
	int j = 0;
	
	while(dupStr[i] != '\0'){
		str[j] = dupStr[i];
		if(dupStr[i] == c){
			j++;
			str[j] = dupStr[i];
		}
		i++;
		j++;
	}
	str[j] = '\0';
}
