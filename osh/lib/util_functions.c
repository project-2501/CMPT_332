#include <ctype.h>
#include <string.h>
#include "util_functions.h"

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
	for (int i = 0; str[i] != '\0'; i++)
	{
		if (str[i] == c)
			count++;
	}
	return count;
}
