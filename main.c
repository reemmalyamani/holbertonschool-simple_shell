#include "shell.h"

int main(int ac, char **av)
{
	(void)ac;

	/* start the shell loop (av[0] is the program name like ./hsh) */
	shell_loop(av[0]);

	return (0);
}
