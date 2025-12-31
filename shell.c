#include "shell.h"
#include <string.h>

/**
 * trim_command - removes spaces + the ending '\n' from getline()
 * @s: string to clean
 *
 * Return: pointer to first “real” character (inside same buffer)
 *
 * Human note: getline keeps the '\n', so "/bin/ls\n" would fail execve
 * unless we remove it.
 */
static char *trim_command(char *s)
{
	size_t i;
	size_t len;

	if (s == NULL)
		return (NULL);

	/* remove trailing newline */
	len = strlen(s);
	if (len > 0 && s[len - 1] == '\n')
		s[len - 1] = '\0';

	/* skip leading spaces/tabs */
	while (*s == ' ' || *s == '\t')
		s++;

	/* remove trailing spaces/tabs */
	len = strlen(s);
	if (len == 0)
		return (s);

	i = len - 1;
	while ((s[i] == ' ' || s[i] == '\t') && i > 0)
	{
		s[i] = '\0';
		i--;
	}

	return (s);
}

/**
 * print_prompt - prints "#cisfun$ " only if interactive
 *
 *  note: isatty() means "is the user typing directly?"
 * If input is coming from a pipe (echo "..." | ./hsh), we DONT show prompt.
 */
void print_prompt(void)
{
	if (isatty(STDIN_FILENO))
		write(STDOUT_FILENO, "#cisfun$ ", 9);
}

/**
 * read_line - reads one line from stdin
 * @line: buffer pointer (getline allocates/expands it)
 * @len: size of buffer
 *
 * Return: bytes read, or -1 on EOF (Ctrl+D)
 */
ssize_t read_line(char **line, size_t *len)
{
	return (getline(line, len, stdin));
}

/**
 * execute_command - forks and runs the command using execve
 * @progname: argv[0] (used for correct error message)
 * @command: cleaned command (like "/bin/ls")
 * @line_no: command counter (needed by checker format)
 *
 * 
 * argv[0] = command, argv[1] = NULL
 */
void execute_command(char *progname, char *command, unsigned long line_no)
{
	pid_t pid;
	char *argv_exec[2];

	argv_exec[0] = command;
	argv_exec[1] = NULL;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return;
	}

	if (pid == 0)
	{
		/* Child: replace this process with the new program */
		if (execve(command, argv_exec, environ) == -1)
		{
			/* Exactly like sh-style "not found" */
			fprintf(stderr, "%s: %lu: %s: not found\n", progname, line_no, command);
			_exit(127);
		}
	}
	else
	{
		/* Parent: wait for child to finish */
		waitpid(pid, NULL, 0);
	}
}

/**
 * shell_loop - the main REPL loop
 * @progname: argv[0], for correct error printing
 *
 * note: loop = prompt -> read -> run -> repeat
 */
void shell_loop(char *progname)
{
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	unsigned long line_no = 0;

	while (1)
	{
		char *cmd;

		print_prompt();

		read = read_line(&line, &len);
		if (read == -1)
		{
			/* Ctrl+D: exit cleanly (and print newline if interactive) */
			if (isatty(STDIN_FILENO))
				write(STDOUT_FILENO, "\n", 1);
			break;
		}

		line_no++;

		cmd = trim_command(line);

		/* If user only pressed Enter or typed spaces, just re-prompt */
		if (cmd[0] == '\0')
			continue;

		execute_command(progname, cmd, line_no);
	}

	free(line);
}
