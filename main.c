#include "shell.h"

void print_prompt(void)
{
    if (isatty(STDIN_FILENO)) {
        write(STDOUT_FILENO, "#cisfun$", 9);
    }
}
ssize_t read_line(char **line, size_t *len)
{
    return getline(line, len, stdin);
}
void execute_command(char *command)
{
    char *args[] = {"/bin/sh", "-c", command, NULL};
    pid_t pid = fork();

    if (pid == 0) {
        execv(args[0], args);
        perror(" No such file or directory\n");
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror(" No such file or directory\n");
    } else {
        wait(NULL);
    }
}
void shell_loop(void)
{
while (1) {
        char *line = NULL;
        size_t len = 0;
        ssize_t read;

        print_prompt();
        read = read_line(&line, &len);
        if (read == -1) 
        {
            free(line);
            break;
        }
        execute_command(line);
        free(line);
    }
}