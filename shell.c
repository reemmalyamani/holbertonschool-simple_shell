#include "shell.h"

/*
 * print_prompt
 * This function just prints the prompt.
 * We only show it when the user is typing directly (interactive mode).
 * If input is coming from a pipe, we do NOT print the prompt.
 */
void print_prompt(void)
{
	if (isatty(STDIN_FILENO))
		write(STDOUT_FILENO, "#cisfun$ ", 9);
}

/*
 * read_line
 * Uses getline to read a full line from standard input.
 * getline handles memory allocation for us.
 * It returns -1 when the user presses Ctrl+D (EOF).
 */
ssize_t read_line(char **line, size_t *len)
{
	return (getline(line, len, stdin));
}

/*
 * execute_command
 * This function runs one command entered by the user.
 *
 * Project rules reminder (why this looks simple):
 * - No pipes, no redirections, no semicolons
 * - No PATH search, so the command must be a full or relative path
 * - We split by spaces only (basic arguments, no quotes handling)
 */
void execute_command(char *line, int *line_count)
{
	pid_t pid;
	char *args[64];
	int i = 0;
	char *token;

	/* Remove the newline added by getline */
	token = strtok(line, "\n");
	if (token == NULL)
		return;

	/* Split the command by spaces and tabs */
	token = strtok(token, " \t");
	while (token != NULL && i < 63)
	{
		args[i++] = token;
		token = strtok(NULL, " \t");
	}
	args[i] = NULL;

	/* If the user just pressed Enter, do nothing */
	if (args[0] == NULL)
		return;

	(*line_count)++;

	pid = fork();
	if (pid == -1)
	{
		/* Fork failed (should not normally happen) */
		perror("fork");
		return;
	}

	if (pid == 0)
	{
		/*
		 * Child process:
		 * execve replaces this process with the new program.
		 * If execve succeeds, this code will never run again.
		 */
		execve(args[0], args, environ);

		/*
		 * If execve returns, it failed.
		 * We print the error exactly like /bin/sh.
		 */
		dprintf(STDERR_FILENO, "%s: %d: %s: not found\n",
			g_progname, *line_count, args[0]);

		_exit(127);
	}
	else
	{
		/*
		 * Parent process:
		 * Wait for the child to finish before showing the prompt again.
		 */
		wait(NULL);
	}
}

/*
 * shell_loop
 * This is the main loop of the shell.
 *
 * Flow:
 * 1. Print prompt
 * 2. Read user input
 * 3. Execute the command
 * 4. Repeat until Ctrl+D
 *
 * We keep the same buffer and free it once at the end
 * to avoid memory errors.
 */
void shell_loop(void)
{
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	int line_count = 0;

	while (1)
	{
		print_prompt();

		read = read_line(&line, &len);
		if (read == -1)
		{
			/* Ctrl + D (EOF) */
			if (isatty(STDIN_FILENO))
				write(STDOUT_FILENO, "\n", 1);
			break;
		}

		execute_command(line, &line_count);
	}

	free(line);
}
