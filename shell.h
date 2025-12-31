#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/* This is the environment list execve needs */
extern char **environ;

void shell_loop(char *progname);
void print_prompt(void);
ssize_t read_line(char **line, size_t *len);
void execute_command(char *progname, char *command, unsigned long line_no);

#endif /* SHELL_H */
