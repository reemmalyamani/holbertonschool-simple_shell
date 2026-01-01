#ifndef SHELL_H
#define SHELL_H

#include "shell.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

/* the environment array that execve needs */
extern char **environ;

/* for task 2-3-4 */
int shell_loop(char *prog_name);
int execute_command(char **argv, char *prog_name, int line_num);
char **split_line(char *line);
char *get_path_value(void);
char *find_command(char *cmd);

#endif /* SHELL_H */
