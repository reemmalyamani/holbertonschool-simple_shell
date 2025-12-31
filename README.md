# Simple Shell

## Description
This project is a simple UNIX command line interpreter written in C.
It displays a prompt, reads a command from standard input, and executes it using fork + execve.

## Requirements / Features
- Displays a prompt and waits for user input (interactive mode)
- Works in non-interactive mode (input piped into the program)
- Executes commands using `fork()` and `execve()`
- Handles command not found errors
- Handles EOF (Ctrl+D)
- No pipes, no redirections, no command separators (`;`)
- Commands are a single word (no arguments) for this early version

## Compilation
```sh
gcc -Wall -Werror -Wextra -pedantic -std=gnu89 *.c -o hsh
##Files 
shell.h : header file (function prototypes, includes)
main.c : program entry point
shell.c : prompt, input reading, execution logic
man_1_simple_shell : man page
AUTHORS : contributors list
