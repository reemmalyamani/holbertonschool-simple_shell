#include "shell.h"

/**
 * print_prompt - prints the prompt only in interactive mode
 */
void print_prompt(void)
{
	/* isatty means: am I talking to a real terminal? */
	if (isatty(STDIN_FILENO))
		write(STDOUT_FILENO, "#cisfun$ ", 9);
}

/**
 * read_line - gets a full line from the user (or stdin)
 * Return: number of chars read, or -1 on EOF/error
 */
ssize_t read_line(char **line, size_t *len)
{
	return (getline(line, len, stdin));
}

/**
 * split_line - split the line into words (command + arguments)
 * Example: "/bin/ls -l /tmp" becomes args[0]="/bin/ls", args[1]="-l", ...
 * Return: NULL-terminated array of strings
 */
char **split_line(char *line)
{
	int bufsize = 64, i = 0;
	char **tokens = malloc(sizeof(char *) * bufsize);
	char *token;

	if (!tokens)
		return (NULL);

	/* remove the last '\n' so it doesn’t mess up execve */
	token = strtok(line, " \t\r\n");
	while (token != NULL)
	{
		tokens[i] = token;
		i++;

		/* if we somehow have too many words, expand */
		if (i >= bufsize)
		{
			bufsize += 64;
			tokens = realloc(tokens, sizeof(char *) * bufsize);
			if (!tokens)
				return (NULL);
		}

		token = strtok(NULL, " \t\r\n");
	}

	tokens[i] = NULL;
	return (tokens);
}

/**
 * free_args - frees the array container (NOT the strings because strtok uses line)
 */
void free_args(char **args)
{
	if (args)
		free(args);
}

/**
 * execute_command - fork + execve the command
 * args[0] must be the program path because Task 3 still doesn't require PATH
 */
void execute_command(char **args, char *prog_name, int line_count)
{
	pid_t pid;
	int status;

	if (!args || !args[0])
		return; /* empty line, just show prompt again */

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return;
	}

	if (pid == 0)
	{
		/*
		 * execve is the core of the project:
		 * - args[0] is the program (ex: /bin/ls)
		 * - args is the argv list (ex: {"ls","-l",NULL})
		 * - environ passes the environment to the new program
		 */
		execve(args[0], args, environ);

		/* if we reached here, execve failed */
		fprintf(stderr, "%s: %d: %s: not found\n", prog_name, line_count, args[0]);
		exit(127);
	}
	else
	{
		/* parent waits so we don't create zombie processes */
		waitpid(pid, &status, 0);
	}
}

/**
 * shell_loop - keeps running until EOF (Ctrl+D) or error
 */
void shell_loop(char *prog_name)
{
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	int line_count = 0;

	while (1)
	{
		char **args;

		line_count++;
		print_prompt();

		/*
		 * Read one full command line.
		 * The command line always ends with '\n' (unless EOF).
		 */
		read = read_line(&line, &len);
		if (read == -1)
		{
			/* Ctrl+D: in interactive, print a newline like real shells */
			if (isatty(STDIN_FILENO))
				write(STDOUT_FILENO, "\n", 1);
			break;
		}

		/*
		 * Task rule: "simple lines" (no pipes/semicolon/redirections).
		 * We are NOT implementing ; | > < etc — we just split by spaces.
		 */
		args = split_line(line);
		if (!args)
			continue;

		execute_command(args, prog_name, line_count);
		free_args(args);
	}

	free(line);
}

