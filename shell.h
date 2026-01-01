#ifndef SHELL_H
#define SHELL_H

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


/* function prototypes */
int main(int ac, char **av);
static int handle_builtin(char **argv, int *last_status);
int shell_loop(char *prog_name);
int execute_command(char **argv, char *prog_name,
int line_num, int *last_status);
char **split_line(char *line);
char *get_path_value(void);
char *find_command(char *cmd);
void child_exec(char *cmd_path, char **argv, char *prog_name);
int parent_wait(pid_t pid, int *last_status);

#endif /* SHELL_H */
