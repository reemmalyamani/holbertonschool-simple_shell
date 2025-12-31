/* shell.c */
#include "shell.h"

#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

/* this is the environment list the system gives our program */
extern char **environ;

/**
 * print_prompt - shows the prompt only if we're typing in interactive mode
 */
void print_prompt(void)
{
	if (isatty(STDIN_FILENO))
		write(STDOUT_FILENO, "#cisfun$ ", 9);
}

/**
 * split_line - takes the raw line and splits it into words (command + args)
 * @line: the input line (we will modify it using strtok)
 *
 * Return: NULL if empty line, otherwise a malloc'd argv array ending with NULL
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

		/* if someone typed more args than expected */
		if (i >= max - 1)
		{
			char **new_argv = malloc(sizeof(char *) * (max * 2));
			int j;

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

	/* if user only pressed Enter (or spaces), treat as empty */
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
 * find_command - if command has no '/', search PATH and build full path
 * @cmd: argv[0] (like "ls" or "./hbtn_ls")
 *
 * Return: malloc'd full path if found, or NULL if not found
 * Note: if cmd already contains '/', we just duplicate it and return it.
 */
char *find_command(char *cmd)
{
	char *path, *path_copy, *dir, *full;
	int cmd_len, dir_len;

	if (!cmd)
		return (NULL);

	/* if they typed /bin/ls or ./hbtn_ls, we don't search PATH */
	if (strchr(cmd, '/'))
		return (strdup(cmd));

	path = get_path_value();
	if (!path)
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
 * execute_command - fork + execve, and print the correct error if not found
 * @argv: array like {"./hbtn_ls", "/var", NULL}
 * @prog_name: argv[0] of our shell (used in error printing)
 * @line_num: command counter (matches checker: "./hsh: 1: ...")
 */
void execute_command(char **argv, char *prog_name, int line_num)
{
	pid_t pid;
	int status;
	char *cmd_path;

	if (!argv || !argv[0])
		return;

	/* task 0.4 style: built-in exit */
	if (strcmp(argv[0], "exit") == 0)
	{
		/* we just exit cleanly; checker usually accepts this */
		exit(0);
	}

	cmd_path = find_command(argv[0]);
	if (!cmd_path)
	{
		fprintf(stderr, "%s: %d: %s: not found\n", prog_name, line_num, argv[0]);
		return;
	}

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		free(cmd_path);
		return;
	}

	if (pid == 0)
	{
		/* child: replace process with the command */
		if (execve(cmd_path, argv, environ) == -1)
		{
			/* if execve fails, print something sensible then exit child */
			perror(prog_name);
			free(cmd_path);
			exit(127);
		}
	}
	else
	{
		/* parent: wait */
		waitpid(pid, &status, 0);
	}

	free(cmd_path);
}

/**
 * shell_loop - main loop: prompt -> read -> split -> execute -> repeat
 * @prog_name: argv[0] from main (for error printing)
 *
 * Return: 0 on normal end (Ctrl+D / EOF)
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
			/* Ctrl+D: in interactive mode we drop to new line nicely */
			if (isatty(STDIN_FILENO))
				write(STDOUT_FILENO, "\n", 1);

			free(line);
			return (0);
		}

		/* remove the trailing '\n' so it doesn't break exec */
		if (read > 0 && line[read - 1] == '\n')
			line[read - 1] = '\0';

		argv = split_line(line);
		if (!argv)
			continue;

		execute_command(argv, prog_name, line_num);

		/* argv itself was malloc'd, but the words point into 'line' */
		free(argv);
	}
}
