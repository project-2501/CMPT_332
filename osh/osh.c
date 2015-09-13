/*
 * Description: Odd Shell implementation for Assignment 1
 *
 * Authors: Taran and Kyle
 * Class:   CMPT 332 - Fall 2015
 *
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/wait.h>

#include "util_functions.h"

#define MAX_TOKENS 200   /* Max tokens allowed for single cmd */

/* Global data */
const char *illegal_chars = ">();:?,!@#$%^&*[]{}'+=~`";

/* Function prototypes */
int numChars(const char *str, const char *c);
void execRedirCmd(char *cmd);
void execPipeCmd(char *cmd);
void execCmd(char *cmd);

int main()
{
	char *c;
	char *cmd = NULL;
	size_t len = 0;

	while(1)
	{
		printf("$osh> ");
		getline(&cmd, &len, stdin);
		cmd[strlen(cmd) -1] = '\0';

		/* Test for presence of illegal characters */
		if (( c = strpbrk(cmd, illegal_chars)) != 0) {
			fprintf(stderr, "Error: Invalid character '%c'\n", c[0]);
			continue;
		}

		/* Classify command and handle accordingly */
		if ( num_chars(cmd, '|') > 0 ) {
			execPipeCmd(cmd);
		}
		else if ( num_chars(cmd, '<') > 0) {
			execRedirCmd(cmd);
		}
		else {
			execCmd(cmd);
		}
	}
	exit(EXIT_SUCCESS);	
}

/* Executes a command that has file redirection */
void execRedirCmd(char *cmd)
{
	//stub
	return;
}

/* Executes a command that contains pipes */
void execPipeCmd(char *cmd)
{
	//stub
	return;
}

/* Executes a regular command */
void execCmd(char *cmd)
{
    int count, status;
    char *tokens[MAX_TOKENS];
	pid_t childPid;

	compress_spaces(cmd);
	count = split(cmd, " \t", tokens);
	tokens[count] = NULL;

	/* Test for builtin commands */
	if (strcmp(tokens[0], "exit") == 0)	{
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
			if (waitpid(childPid, &status, 0) == -1) {
				fprintf(stderr, "Error: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}
	}
	return;
}
