#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/wait.h>

#include "util_functions.h"

#define MAX_TOKENS 200

int main()
{
    int count, status;
    char *tokens[MAX_TOKENS];
	char *c;
	pid_t childPid;
	const char *illegal_chars = "<();:?,!@#$%^&*[]{}'+=~`";
	char *string = NULL;
	size_t len = 0;

	while(1)
	{
		printf("$osh> ");
		getline(&string, &len, stdin);
		string[strlen(string) -1] = '\0';

		/* Test for presence of illegal characters */
		if (( c = strpbrk(string, illegal_chars)) != 0) {
			fprintf(stderr, "Error: Invalid character '%c'\n", c[0]);
			exit(EXIT_FAILURE);
		}
		
		compress_spaces(string);
		count = split(string, " \t", tokens);
		tokens[count] = NULL;

		if (strcmp(tokens[0], "exit") == 0)
		{
			printf("Goodbye!\n");
			exit(EXIT_SUCCESS);
		}

		switch(childPid = fork()) {
			case -1: /* Error */
				fprintf(stderr, "Error: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			
			case 0: /* Child */
				execvp(tokens[0], tokens);
				fprintf(stderr, "Error: %s\n", strerror(errno));
				exit(EXIT_FAILURE);

			default: /* Parent */
				if (waitpid(childPid, &status, 0) == -1)
				{
					fprintf(stderr, "Error: %s\n", strerror(errno));
					exit(EXIT_FAILURE);
				}
		}
	}
	exit(EXIT_SUCCESS);	
}
