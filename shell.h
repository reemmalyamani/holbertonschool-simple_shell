#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

/* environ is needed because execve needs environment */
extern char **environ;

/* We store argv[0] so errors print the correct program name */
extern char *g_progname;

void print_prompt(void);
ssize_t read_line(char **line, size_t *len);
void execute_command(char *line, int *line_count);
void shell_loop(void);

#endif /* SHELL_H */
