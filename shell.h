#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/* the environment array that execve needs */
extern char **environ;

/* for task 2-3-4 */
int shell_loop(char *prog_name);
void execute_command(char **argv, char *prog_name, int line_num);
char **split_line(char *line);
char *get_path_value(void);
char *find_command(char *cmd);

#endif /* SHELL_H */
