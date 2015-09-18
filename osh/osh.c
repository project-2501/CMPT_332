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
	int num_cmds, num_pipes, status, i, j, q;
	char *tokens[MAX_TOKENS];
	pid_t childpid;

	/* Format and tokenize the pipe command */
	compress_spaces(cmd);
	num_cmds = split(cmd, "|", tokens);
	tokens[num_cmds] = NULL;
	num_pipes = num_cmds - 1;

	/* Create the pipe file descriptor array */
	int pipefds[2*num_pipes];
	for (i=0; i < num_pipes; i++) {
		if ( pipe(pipefds + 2*i) < 0) {
			fprintf(stderr, "Error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	/* Reverse the order of the token elements to get 'odd' shell style */
	reverse_array(tokens, num_cmds + 1);

	i = 0; /* Index into tokens[] array */
	j = 0; /* Index into pipefds[] array */
	while(tokens[i])
	{
		switch(childpid = fork()) {
			case -1: /* Error */
				fprintf(stderr, "Error: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			
			case 0: /* Child implements the pipe mill */

				/* If this command is not the last one, redirect stdout */
				if (i < num_pipes) {
					if (num_chars(tokens[i], '<') > 0) {
						fprintf(stderr, "Error: illegal redirection in '%s'\n", tokens[i]);
						exit(EXIT_FAILURE);
					}
					if (dup2(pipefds[j + 1], 1) < 0) { 
						fprintf(stderr, "Error: %s\n", strerror(errno));
						exit(EXIT_FAILURE);
					}
				}

				/* If this command is not the first, redirect stdin */
				if (j != 0) {
					if (dup2(pipefds[j - 2], 0) < 0) { 
						fprintf(stderr, "Error: %s\n", strerror(errno));
						exit(EXIT_FAILURE);
					}
				}

				/* Close all of this childs pipefd handles */
				for (q = 0; q < 2*num_pipes; q++)
					close(pipefds[q]);

				/* Execute the command - handle if final command has < */
				tokens[i] = trimwhitespace(tokens[i]);
				if ( (i == num_pipes) && (num_chars(tokens[i], '<') > 0) )
					execRedirCmd(tokens[i]);
				else
					execCmd(tokens[i], NULL);
				
				/* This child can now exit */
				exit(EXIT_SUCCESS);
		}

		/* Increment the indices */
		i++; j = j + 2;
	}

	/* Close all of the parents pipefds */
	for (i = 0; i < 2 * num_pipes; i++)
		close(pipefds[i]);

	/* Wait on all of the child processes forked */
	for (i = 0; i < num_cmds; i++) {
		if (wait(&status) == -1) {
		fprintf(stderr, "Error in pipe: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
		}
	}

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
				open(file, O_WRONLY | O_CREAT | O_TRUNC, 
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
				fprintf(stderr, "Error in exec: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}
	}
	return;
}
