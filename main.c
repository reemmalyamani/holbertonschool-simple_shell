#include "shell.h"

/**
 * main - Entry point
 * @argc: argument count (unused)
 * @argv: argument vector (we use argv[0] for error messages)
 *
 * Return: 0 on success
 */
int main(int argc, char **argv)
{
	(void)argc;

	shell_loop(argv[0]); /* argv[0] is the program name (ex: "./hsh") */
	return (0);
}
