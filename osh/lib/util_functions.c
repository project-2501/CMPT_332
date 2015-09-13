#include <ctype.h>
#include <string.h>
#include "util_functions.h"

/* Utility function to compress multiple whitespace to a single space */
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
}

/* Utility function to split string by token and return char pointer array */
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

