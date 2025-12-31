#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>  
#include <unistd.h>

void  shell_loop(void);
void print_prompt(void);
ssize_t read_line(char **line, size_t *len);
void execute_command(char *command);

#endif /* SHELL_H */
