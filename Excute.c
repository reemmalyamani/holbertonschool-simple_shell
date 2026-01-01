#include "shell.h"
/**
* child_exec - executes the command in the child process
* @cmd_path: full path to the command
* @argv: argument array for the command
* @prog_name: name of the shell program
* Description: Sets default signal handling and executes the command
* using execve(). If execve fails
*/
static void child_exec(char *cmd_path, char **argv, char *prog_name)
{
	signal(SIGINT, SIG_DFL);
	if (execve(cmd_path, argv, environ) == -1)
	{
		perror(prog_name);
		free(cmd_path);
		_exit(127);
	}
}
/**
* parent_wait - waits for the child process to finish
* @pid: process ID of the child
* @last_status: pointer to store the last command's exit status
* Description: Waits for the child process to complete and
* retrieves its exit status.
* Return: 0 on success, -1 on waitpid error
*/
static int parent_wait(pid_t pid, int *last_status)
{
	int status;

	if (waitpid(pid, &status, 0) == -1)
	{
		perror("waitpid");
		*last_status = 1;
		return (-1);
	}

	if (WIFEXITED(status))
		*last_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		*last_status = 128 + WTERMSIG(status);
	else
		*last_status = 1;

	return (0);
}

/**
 * execute_command - executes a command by forking and running it
 * @argv: array of arguments for the command
 * @prog_name: name of the shell program
 * @line_num: line number of the command
 * @last_status: pointer to store the last command's exit status
 *
 * Description: Handles built-in commands directly. For other commands,
 * forks a child process to execute the program.
 *
 * Return: 1 if exit was called, 0 otherwise
 */
int execute_command(char **argv, char *prog_name,
int line_num, int *last_status)
{
	pid_t pid;
	char *cmd_path;
	int status;

	if (!argv || !argv[0])
		return (0);

	status = handle_builtin(argv, last_status);
	if (status != -1)
		return (status);

	cmd_path = find_command(argv[0]);
	if (!cmd_path)
	{
		fprintf(stderr, "%s: %d: %s: not found\n",
			prog_name, line_num, argv[0]);
		*last_status = 127;
		return (0);
	}

	signal(SIGINT, SIG_IGN);
	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		free(cmd_path);
		*last_status = 1;
		return (0);
	}

	if (pid == 0)
	{
		child_exec(cmd_path, argv, prog_name);
	}
	else
	{
		parent_wait(pid, last_status);
	}
	free(cmd_path);
	return (0);
}
