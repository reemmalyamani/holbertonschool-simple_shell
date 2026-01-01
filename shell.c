/* shell.c */
#include "shell.h"

int last_status = 0;

/* this is the environment list the system gives our program */
extern char **environ;

/**
 * print_prompt - prints the shell prompt in interactive mode
 *
 * Description: This function checks if stdin is connected to a terminal
 * using isatty(). If it is, it writes the prompt "#cisfun$ " to stdout.
 * This provides a visual indicator that the shell is ready for input.
 */
void print_prompt(void)
{
	if (isatty(STDIN_FILENO))
		write(STDOUT_FILENO, "#cisfun$ ", 9);
}

/**
 * split_line - tokenizes the input line into arguments
 * @line: pointer to the input line to tokenize
 *
 * Description: Uses strtok() to split the input string by spaces and tabs.
 * Allocates memory for the argument array, resizing as needed to accommodate
 * all arguments. Returns NULL for empty lines or allocation failures.
 *
 * Return: NULL if line is empty, otherwise a dynamically allocated array
 *         of strings ending with a NULL pointer
 */
char **split_line(char *line)
{
	char **argv;
	int i = 0;
	int max = 64;
	char *token;

	argv = malloc(sizeof(char *) * max);
	if (!argv)
		return (NULL);

	token = strtok(line, " \t");
	while (token)
	{
		argv[i++] = token;

		/* reallocate if we run out of space for arguments */
		if (i >= max - 1)
		{
			char **new_argv;
			int j;

			new_argv = malloc(sizeof(char *) * (max * 2));
			if (!new_argv)
			{
				free(argv);
				return (NULL);
			}

			for (j = 0; j < i; j++)
				new_argv[j] = argv[j];

			free(argv);
			argv = new_argv;
			max *= 2;
		}

		token = strtok(NULL, " \t");
	}

	argv[i] = NULL;

	/* free and return NULL if the input was empty or only whitespace */
	if (i == 0)
	{
		free(argv);
		return (NULL);
	}

	return (argv);
}

/**
 * get_path_value - retrieves the value of PATH from environment
 *
 * Description: Manually searches the environ array for an entry starting
 * with "PATH=". This is required because getenv() is not allowed.
 *
 * Return: pointer to the PATH value string (not newly allocated), or
 *         NULL if PATH is not found
 */
char *get_path_value(void)
{
	int i = 0;

	while (environ[i])
	{
		if (strncmp(environ[i], "PATH=", 5) == 0)
			return (environ[i] + 5);
		i++;
	}
	return (NULL);
}

/**
 * find_command - locates an executable command
 * @cmd: the command name or path to search for
 *
 * Description: If cmd contains a '/', verifies the exact path exists and
 * is executable. Otherwise, searches each directory in PATH for an
 * executable file matching cmd. Allocates memory for the full path.
 *
 * Return: newly allocated string containing the full path, or NULL if
 *         the command cannot be found
 */
char *find_command(char *cmd)
{
	char *path;
	char *path_copy;
	char *dir;
	char *full;
	int cmd_len;
	int dir_len;

	if (!cmd)
		return (NULL);

	/* if the command contains a slash, use the path as-is */
	if (strchr(cmd, '/'))
	{
		if (access(cmd, X_OK) == 0)
			return (strdup(cmd));
		return (NULL);
	}

	path = get_path_value();
	if (!path || path[0] == '\0')
		return (NULL);

	path_copy = strdup(path);
	if (!path_copy)
		return (NULL);

	cmd_len = (int)strlen(cmd);

	dir = strtok(path_copy, ":");
	while (dir)
	{
		dir_len = (int)strlen(dir);

		full = malloc(dir_len + 1 + cmd_len + 1);
		if (!full)
		{
			free(path_copy);
			return (NULL);
		}

		strcpy(full, dir);
		strcat(full, "/");
		strcat(full, cmd);

		if (access(full, X_OK) == 0)
		{
			free(path_copy);
			return (full);
		}

		free(full);
		dir = strtok(NULL, ":");
	}

	free(path_copy);
	return (NULL);
}

/**
 * execute_command - executes a command by forking and running it
 * @argv: array of arguments for the command
 * @prog_name: name of the shell program (for error messages)
 * @line_num: line number of the command (for error messages)
 *
 * Description: Handles built-in commands (exit, env) directly.
 * For other commands, forks a child process to execute the program.
 * Sets last_status to indicate the outcome of the command.
 *
 * Return: 1 if exit was called (to signal shell_loop to exit), 0 otherwise
 */
int execute_command(char **argv, char *prog_name, int line_num)
{
	pid_t pid;
	int status;
	char *cmd_path;

	if (!argv || !argv[0])
		return (0);

	/* handle the exit built-in */
	if (strcmp(argv[0], "exit") == 0)
		return (1);

	/* handle the env built-in */
	if (strcmp(argv[0], "env") == 0)
	{
		int i = 0;

		while (environ[i])
		{
			printf("%s\n", environ[i]);
			i++;
		}
		last_status = 0;
		return (0);
	}

	/* locate the command before attempting to execute it */
	cmd_path = find_command(argv[0]);
	if (!cmd_path)
	{
		fprintf(stderr, "%s: %d: %s: not found\n",
			prog_name, line_num, argv[0]);
		last_status = 127;
		return (0);
	}

	signal(SIGINT, SIG_IGN);
	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		free(cmd_path);
		last_status = 1;
		return (0);
	}

	if (pid == 0)
	{
		signal(SIGINT, SIG_DFL);
		if (execve(cmd_path, argv, environ) == -1)
		{
			perror(prog_name);
			free(cmd_path);
			_exit(127);
		}
	}
	else
	{
		if (waitpid(pid, &status, 0) == -1)
		{
			perror("waitpid");
			last_status = 1;
		}
		else
		{
			if (WIFEXITED(status))
				last_status = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
				last_status = 128 + WTERMSIG(status);
			else
				last_status = 1;
		}
	}

	free(cmd_path);
	return (0);
}

/**
 * shell_loop - main shell loop that processes commands
 * @prog_name: name of the shell program (for error messages)
 *
 * Description: Continuously reads input, parses it, and executes commands
 * until end-of-file is encountered or the exit built-in is called.
 * Handles both interactive and non-interactive modes.
 *
 * Return: the exit status of the last executed command
 */
int shell_loop(char *prog_name)
{
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	int line_num = 0;
	char **argv;

	while (1)
	{
		line_num++;
		print_prompt();

		read = getline(&line, &len, stdin);
		if (read == -1)
		{
			/* print newline on Ctrl+D in interactive mode */
			if (isatty(STDIN_FILENO))
				write(STDOUT_FILENO, "\n", 1);

			break;
		}

		/* remove trailing newline character */
		if (read > 0 && line[read - 1] == '\n')
			line[read - 1] = '\0';

		argv = split_line(line);
		if (!argv)
			continue;

		if (execute_command(argv, prog_name, line_num) == 1)
		{
			free(argv);
			free(line);
			exit(last_status);
		}

		/* free the argv array (strings point into line, don't free them) */
		free(argv);
	}

	/* return last_status for non-interactive mode exit */
	free(line);
	return (last_status);
}
