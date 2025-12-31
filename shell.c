#include "shell.h"

void print_prompt(void)
{
    /*
     * TASK:
     * "Display a prompt and wait for the user to type a command."
     *
     * We only print the prompt if stdin is a terminal.
     * This avoids printing prompts in non-interactive mode (pipes).
     */
    if (isatty(STDIN_FILENO))
    {
        write(STDOUT_FILENO, "#cisfun$ ", 9);
    }
}

ssize_t read_line(char **line, size_t *len)
{
    /*
     * TASk:
     * "A command line always ends with a new line."
     *
     * getline waits for the user to press Enter
     * and includes the newline character in the buffer.
     */
    return getline(line, len, stdin);
}

void execute_command(char *command, char *prog_name, int line_number)
{
    pid_t pid;
    char *argv_exec[2];

    /*
     * Remove the newline added by getline.
     * execve expects a clean executable path.
     */
    command[strcspn(command, "\n")] = '\0';

    /*
     * If the user presses Enter without typing anything,
     * we simply return and show the prompt again.
     */
    if (command[0] == '\0')
        return;

    /*
     * TASK:
     * - "The command lines are made only of one word."
     * - "No arguments will be passed to programs."
     *
     * We dont split the command.
     * argv contains only the command  and NULL.
     */
    argv_exec[0] = command;
    argv_exec[1] = NULL;

    /*
     * TASK REQUIREMENT:
     * "How to create processes"
     *
     * fork creates a child process.
     */
    pid = fork();

    if (pid == 0)
    {
        /*
         * CHILD PROCESS
         *
         * TASK:
         * "execve will be the core part of your Shell,
         *  don’t forget to pass the environ to it."
         *
         * execve replaces the child with the new program.
         */
        execve(command, argv_exec, environ);

        /*
         * If execve returns, the executable was not found.
         *
         * TASK:
         * "If an executable cannot be found, print an error message
         *  and display the prompt again."
         *
         * Error format matches /bin/sh.
         */
        dprintf(STDERR_FILENO, "%s: %d: %s: not found\n",
                prog_name, line_number, command);

        exit(127); /* Standard exit code for command not found */
    }
    else if (pid < 0)
    {
        /*
         * fork failed due to a system error.
         * We print the system error message.
         */
        perror("fork");
    }
    else
    {
        /*
         * PARENT PROCESS
         *
         * TASK REQUIREMENT:
         * "How to suspend the execution of a process
         *  until one of its children terminates"
         *
         * The parent waits for the child to finish
         * before showing the prompt again.
         */
        wait(NULL);
    }
}

void shell_loop(void)
{
    int line_number = 0;

    /*
     * TASK:
     * "Display a prompt and wait for the user to type a command.
     *  The prompt is displayed again each time a command has been executed."
     *
     * This infinite loop keeps the shell running.
     */
    while (1)
    {
        char *line = NULL;
        size_t len = 0;
        ssize_t read;

        print_prompt();
        read = read_line(&line, &len);

        /*
         * TASK:
         * "You have to handle the end of file condition (Ctrl+D)"
         *
         * getline returns -1 on EOF.
         * In interactive mode, we print a newline before exiting.
         */
        if (read == -1)
        {
            if (isatty(STDIN_FILENO))
                write(STDOUT_FILENO, "\n", 1);

            free(line);
            break;
        }

        /*
         * Increment line counter so error messages
         * match the behavior of /bin/sh.
         */
        line_number++;

        /*
         * TASK :
         * - No semicolons
         * - No pipes
         * - No redirections
         * - No advanced features
         *
         * We treat the entire input as one command string.
         * Anything with special characters will fail naturally.
         */
        execute_command(line, "./hsh", line_number);

        free(line);
    }
}
