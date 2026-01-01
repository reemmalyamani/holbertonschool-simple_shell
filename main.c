#include "shell.h"
/**
* main - entry point for our simple shell
* @ac: argument count (not used)
* @av: argument vector (program name is av[0])
* Return: 0 on success
*/
int main(int ac, char **av)
{
	(void)ac;

	/* start the shell loop (av[0] is the program name like ./hsh) */
return (shell_loop(av[0]));
}
