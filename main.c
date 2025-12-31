#include "shell.h"

/**
 * main - Entry point of simple program
 * @argc: argument count (not used for this project)
 * @argv: argument vector (we keep it to match main prototype)
 *
 * Return: 0 on success
 */
int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	/* Start the shell loop (prompt -> read -> execute -> repeat) */
	shell_loop();

	return (0);
}
