# Simple Shell

> A minimal UNIX command-line interpreter written in C from scratch. Built with
> bare metal system calls and the stubborn determination of a first-semester
> Holberton student.

---

## Table of Contents

- [What Is a Shell?](#what-is-a-shell)
- [How It Works Internally](#how-it-works-internally)
- [Built-in Commands](#built-in-commands)
- [PATH Handling](#path-handling)
- [Exit Status Behavior](#exit-status-behavior)
- [Compilation](#compilation)
- [Usage](#usage)
- [Project Structure](#project-structure)
- [Learning Goals](#learning-goals)
- [Limitations](#limitations)
- [Authors](#authors)

---

## What Is a Shell?

A shell is both a command interpreter and an interface between you and the
operating system. Think of it as a **restaurant waiter**:

1. You place your order (type a command)
2. The waiter relays it to the kitchen (the kernel)
3. The kitchen prepares the food (runs the program)
4. The waiter brings it back to you

When you type `ls -l`, the shell orchestrates this entire performance:

| Step | Action | System Call |
|------|--------|-------------|
| 1 | Reads your input | `getline()` |
| 2 | Parses into arguments | `strtok()` |
| 3 | Searches for executable | `access()` |
| 4 | Creates a new process | `fork()` |
| 5 | Runs the command | `execve()` |
| 6 | Waits for completion | `waitpid()` |

This project implements that pipeline using only low-level system calls—no
`system()`, no `getenv()`, just raw UNIX primitives.

---

## How It Works Internally

### The Shell Loop

At its core, the shell runs a simple infinite loop:

```c
while (1) {
    print_prompt();
    read_line();
    parse_line();
    execute_command();
}
```

This loop runs forever until you type `exit`. A shell never truly retires—it's
always ready for the next command.

### Step 1: Reading Input

The shell uses `getline()` to read a complete line from stdin. In interactive
mode, it prints a prompt (`#cisfun$ `). In piped mode, no prompt appears.

### Step 2: Parsing

Input gets tokenized using `strtok()`:

```
"ls -la /tmp"  ──►  argv[0] = "ls"
                    argv[1] = "-la"
                    argv[2] = "/tmp"
                    argv[3] = NULL
```

Dynamic allocation handles any number of arguments. Empty lines are ignored.

### Step 3: Finding the Command

The shell searches for executables in two scenarios:

| If command contains `/` | If command is a bare name |
|------------------------|---------------------------|
| Check that exact path | Search PATH directories |
| Uses `access()` to verify | Tries each dir + command |
| Example: `./script.sh` | Example: `ls` |

PATH parsing is done manually via `environ`—no `getenv()` shortcuts.

### Step 4: Execution (The Fork-Exec Dance)

```
Parent (Shell)                              Child (New Process)
───────────────                             ─────────────────
     │                                            │
     │  fork() ─────────────────────────────────► │
     │                                            │ execve()
     │                                            │ (becomes ls, cat, etc.)
     │ ◄──────────────────────────────────── waitpid()
     │
  resumes loop
```

1. **Check builtins** (`exit`, `env`) — handled without forking
2. **Fork** — creates a child process
3. **Exec** — child replaces itself with the target program
4. **Wait** — parent pauses until child finishes

### Step 5: Signal Handling

- **Parent** ignores `SIGINT` — Ctrl+C won't kill the shell
- **Child** has default signal handling — Ctrl+C terminates the command

---

## Built-in Commands

These run inside the shell process—no fork required.

### `exit`

```bash
#cisfun$ exit
```

Terminates the shell, returning the exit status of the last command.

### `env`

```bash
#cisfun$ env
USER=student
PATH=/usr/local/bin:/bin:/usr/bin
HOME=/home/student
```

Prints all environment variables by iterating through `environ` manually.

---

## PATH Handling

The shell manually parses PATH from `environ`:

```
environ: ["USER=student", "PATH=/bin:/usr/bin", "HOME=/home/..."]
                     │
                     ▼
           Find "PATH=" prefix
                     │
                     ▼
    Split by ":": ["/bin", "/usr/bin"]
                     │
                     ▼
    For each dir: dir + "/" + command
    Try access(full_path, X_OK)
                     │
                     ▼
         Return first match found
```

If no executable is found → exit status **127** with "not found" error.

---

## Exit Status Behavior

The `last_status` variable tracks the most recent command's outcome:

| Scenario | Exit Status |
|----------|-------------|
| Command succeeded | 0 (or command's return value) |
| Command not found | 127 |
| Killed by signal N | 128 + N |
| Fork/other error | 1 |

This follows UNIX conventions for script compatibility.

---

## Compilation

```bash
gcc -Wall -Werror -Wextra -pedantic -std=gnu89 *.c -o hsh
```

| Flag | Purpose |
|------|---------|
| `-Wall -Werror -Wextra` | All warnings treated as errors |
| `-pedantic` | Strict ISO C compliance |
| `-std=gnu89` | GNU C89 standard |

---

## Usage

### Interactive Mode

```bash
$ ./hsh
#cisfun$ ls -la
#cisfun$ pwd
/home/student/simple_shell
#cisfun$ echo Hello, World!
Hello, World!
#cisfun$ env | head -3
#cisfun$ exit
$
```

### Non-Interactive Mode

```bash
$ echo "ls /tmp" | ./hsh
file1.txt  error.log  temp.dat

$ cat commands.txt | ./hsh
```

---

## Project Structure

```
simple_shell/
├── shell.c          # Core implementation
│   ├── shell_loop()      # Main loop
│   ├── execute_command() # Fork/exec/wait
│   ├── split_line()      # Tokenization
│   ├── find_command()    # PATH search
│   ├── get_path_value()  # Extract PATH
│   └── print_prompt()    # Interactive prompt
├── shell.h          # Header with prototypes
├── main.c           # Entry point
└── AUTHORS          # Contributors
```

---

## Learning Goals

This project builds mastery in:

- **Process Management** — `fork`, `execve`, `wait` lifecycle
- **System Calls** — Direct kernel communication
- **Memory Management** — `malloc`/`free` without leaks
- **Environment Handling** — Manual `environ` parsing
- **Signal Handling** — `SIGINT` and process survival
- **Error Handling** — Every return value checked
- **Code Style** — Betty-compliant readability
- **UNIX Philosophy** — Composable single-purpose tools

---

## Limitations

| Feature | Status |
|---------|--------|
| Pipes (`\|`) | Not implemented |
| Redirection (`>`, `<`) | Not implemented |
| Wildcards (`*`, `?`) | Not implemented |
| Background jobs (`&`) | Not implemented |
| Environment expansion (`$VAR`) | Not implemented |
| Command history | Not implemented |
| Aliases/Functions | Not implemented |

These await future projects!

---

## Authors

- **Reem Alyamani** <alyamanireeem@gmail.com>
- **Aljawharah Alammar** <12423@holbertonstudents.com>

Built with coffee, determination, and too many `printf("debug\n")` statements.

---

## References

- `man 2 fork`, `man 2 execve`, `man 3 environ` — System call documentation
- *The C Programming Language* — Kernighan & Ritchie
- *Advanced Programming in the UNIX Environment* — W. Richard Stevens
- Holberton School Curriculum Materials

---

> "The only way to learn a new programming language is by writing programs in it."
> — Dennis Ritchie

Now go write some code.
