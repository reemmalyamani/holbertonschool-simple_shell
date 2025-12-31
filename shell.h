k#ifndef SHELL_H
#define SHELL_H

/* Standard libraries for input, processes, and memory */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

/*
 * environ is provided by the system.
 * We pass it to execve so the program we run
 * inherits the same environment as our shell.
 * //we talked about this 
 */
extern char **environ;

/* This is the main loop that  keeps the shell running */
void shell_loop(void);

/* Just prints the prompt when  in interactive mode */
void print_prompt(void);

/* Wrapper around getline to read user input
* ask me about this later !!!
 */
ssize_t read_line(char **line, size_t *len);

/*
 * Runs the command the user typed.
 * prog_name and line_number are used only
 * to print errors
 */
void execute_command(char *command, char *prog_name, int line_number);

#endif /* SHELL_H */

