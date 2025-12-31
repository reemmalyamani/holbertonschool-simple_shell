#include "shell.h"
#include <sys/wait.h>

/* Declare environ so execve can use it */
extern char **environ;

/**
 * print_prompt - Displays the shell prompt
 */
void print_prompt(void)
{
	if (isatty(STDIN_FILENO))
		write(STDOUT_FILENO, "#cisfun$ ", 9);
}

/**
 * read_line - Reads input from the user
 */
ssize_t read_line(char **line, size_t *len)
{
	return (getline(line, len, stdin));
}

/**
 * execute_command - Executes a command entered by the user
 * @command: command to execute (must be a full path like /bin/ls)
 */
void execute_command(char *command)
{
	pid_t pid;
	char *args[2];

	/* Simple command: no arguments */
	args[0] = command;
	args[1] = NULL;

	pid = fork();
	if (pid == 0)
	{
		/* Child process */
		execve(command, args, environ);

		/* execve only returns if it fails */
		perror("hsh");
		exit(EXIT_FAILURE);
	}
	else if (pid < 0)
	{
		/* Fork failed */
		perror("hsh");
	}
	else
	{
		/* Parent waits for child */
		wait(NULL);
	}
}

/**
 * shell_loop - Main shell loop
 */
void shell_loop(void)
{
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	while (1)
	{
		print_prompt();

		read = read_line(&line, &len);
		if (read == -1)
		{
			/* Ctrl + D */
			free(line);
			break;
		}

		execute_command(line);
	}

	free(line);
}
