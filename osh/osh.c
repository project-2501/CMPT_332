/*
 * Description: Odd Shell implementation for Assignment 1
 *
 * Authors: Taran and Kyle
 * Class:   CMPT 332 - Fall 2015
 *
*/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "util_functions.h"

#define MAX_TOKENS 200   /* Max tokens allowed for single cmd */

/* Global data */
const char *illegal_chars = ">();:?,!@#$%^&*[]{}'+=~`";

/* Function prototypes */
static void execRedirCmd(char *cmd);
static void execPipeCmd(char *cmd);
static void execCmd(char *cmd, char *file);

int main()
{
	char *c;
	char *cmd = NULL;
	int redirCount = 0;

	rl_bind_key('\t', rl_abort); /* Disable auto-complete */

	while(( cmd = readline("$osh> ")) != NULL)
	{
		if (cmd[0] == 0) /* Ignore empty cmd's */
			continue; 
		else 
			add_history(cmd);

		/* Test for presence of illegal user input */
		redirCount = num_chars(cmd, '<');
		if (( c = strpbrk(cmd, illegal_chars)) != 0) {
			fprintf(stderr, "Error: Invalid character '%c'\n", c[0]);
			continue;
		}
		else if (redirCount > 1) {
			fprintf(stderr, "Error: Too many redirects '<'\n");
			continue;
		}

		/* Classify command and handle accordingly */
		if ( num_chars(cmd, '|') > 0 ) {
			execPipeCmd(cmd);
		}
		else if ( redirCount > 0) {
			execRedirCmd(cmd);
		}
		else {
			execCmd(cmd, NULL);
		}
	}
	exit(EXIT_SUCCESS);	
}

/* Executes a command that has file redirection */
static void
execRedirCmd(char *cmd)
{
	int count;
	char *tokens[3];
	
	/* Parse and format cmd and file strings */
	compress_spaces(cmd);
	count = split(cmd, "<", tokens);
	tokens[count] = NULL;
	tokens[1] = trimwhitespace(tokens[1]);
	tokens[0] = trimwhitespace(tokens[0]);

	/* Execute the redirection command */
	execCmd(tokens[1], tokens[0]);
	
	return;
}

/* Executes a command that contains pipes */
static void
execPipeCmd(char *cmd)
{
	//stub
	return;
}

/* Executes a regular command */
static void
execCmd(char *cmd, char *file)
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
			if (file != NULL) /* Provide stdout redirection to file */
			{
				close(STDOUT_FILENO);
				open(file, O_WRONLY | O_CREAT, 
						S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
				execvp(tokens[0], tokens);
				fprintf(stderr, "Error: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}
			else if (file == NULL) /* Regular command execution */
			{
				execvp(tokens[0], tokens);
				fprintf(stderr, "Error: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}

		default: /* Parent */
			if (waitpid(childPid, &status, 0) == -1) {
				fprintf(stderr, "Error: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}
	}
	return;
}
