/* shell.c */
#include "shell.h"

/**
* handle_builtin - processes built-in commands
* @argv: argument array to check
* @last_status: pointer to store the last command's exit status
*
* Description: Checks if the command is a built-in (exit or env) and
* executes it directly without forking.
*
* Return: 1 if exit was called, 0 if env was executed, -1 otherwise
*/
static int handle_builtin(char **argv, int *last_status)
{
	if (strcmp(argv[0], "exit") == 0)
		return (1);

	if (strcmp(argv[0], "env") == 0)
	{
		int i = 0;

		while (environ[i])
		{
			printf("%s\n", environ[i]);
			i++;
		}
		*last_status = 0;
		return (0);
	}

	return (-1);
}

/**
* split_line - tokenizes the input line into arguments
* @line: pointer to the input line to tokenize
*
* Description: Uses strtok() to split the input string by spaces and tabs.
* Allocates memory for the argument array, resizing as needed.
*
* Return: NULL if line is empty, otherwise a dynamically allocated array
*/
char **split_line(char *line)
{
	char **argv;
	int i = 0, max = 64;
	char *token;

	argv = malloc(sizeof(char *) * max);
	if (!argv)
		return (NULL);
	token = strtok(line, " \t");
	while (token)
	{
		argv[i++] = token;

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
	if (i == 0)
	{
		free(argv);
		return (NULL);
	}
	return (argv);
}
/**
* get_path_value - finds PATH=... inside environ (since getenv isn't allowed)
* Return: pointer to the PATH value (not malloc'd), or NULL if not found
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
* is executable. Otherwise, searches each directory in PATH.
*
* Return: newly allocated string containing the full path
* or NULL if not found
*/
char *find_command(char *cmd)
{
	char *path, *path_copy, *dir, *full;
	int cmd_len, dir_len;

	if (!cmd)
		return (NULL);
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
			free(path_copy);
			return (NULL);
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
* shell_loop - main shell loop that processes commands
* @prog_name: name of the shell program
*
* Description: Continuously reads input, parses it, and executes commands
* until end-of-file is encountered or exit is called.
*
* Return: the exit status of the last executed command
*/
int shell_loop(char *prog_name)
{
	char *line = NULL, **argv;

	size_t len = 0;
	ssize_t read;
	int line_num = 0;

	int last_status = 0;

	while (1)
	{
		line_num++;
		if (isatty(STDIN_FILENO))
			write(STDOUT_FILENO, "#cisfun$ ", 9);

		read = getline(&line, &len, stdin);
		if (read == -1)
		{
			if (isatty(STDIN_FILENO))
				write(STDOUT_FILENO, "\n", 1);

			break;
		}
		if (read > 0 && line[read - 1] == '\n')
			line[read - 1] = '\0';

		argv = split_line(line);
		if (!argv)
			continue;
		if (execute_command(argv, prog_name, line_num, &last_status) == 1)
		{
			free(argv);
			free(line);
			exit(last_status);
		}
		free(argv);
	}
	free(line);
	return (last_status);
}
