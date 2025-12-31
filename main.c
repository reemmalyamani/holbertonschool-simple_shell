#include "shell.h"

/* define the global program name */
char *g_progname = NULL;

int main(int ac, char **av)
{
	(void)ac;

	g_progname = av[0];
	shell_loop();
	return (0);
}
