#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

/* we need environ because execve expects the environment */
extern char **environ;

void shell_loop(char *prog_name);
void print_prompt(void);
ssize_t read_line(char **line, size_t *len);
char **split_line(char *line);
void execute_command(char **args, char *prog_name, int line_count);
void free_args(char **args);

#endif /* SHELL_H */
