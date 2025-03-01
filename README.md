# Unix Shell

## Overview
This project implements a simple Unix shell in C that supports built-in commands, command execution with forking, input/output redirection, and pipes. The shell reads user input, parses commands, and executes them accordingly.

## Features
- **Built-in commands:**
  - `help` - Displays available commands.
  - `exit` - Exits the shell.
  - `cd <dir>` - Changes the current directory.
  - `mkdir <dir>` - Creates a new directory.
  - `!!` - Repeats the last executed command.
- **Command Execution:**
  - Runs standard Unix commands using `execvp()`.
  - Supports single commands and multiple arguments.
- **Input/Output Redirection:**
  - Uses `>` to redirect output to a file.
  - Uses `<` to read input from a file.
- **Piping Support:**
  - Recognizes the `|` operator to pass output between commands.
- **Special Commands:**
  - `ECHO` - Prints commands without redirection or pipes.
  - `IO` - Prints filenames used for input/output redirection.
  - `PIPE` - Displays pipeline execution steps.

## Installation & Compilation
To compile the shell, run:
```sh
gcc -o shell shell.c
```

To start the shell, run:
```sh
./shell
```

## Usage Examples
### Running Basic Commands
```sh
$ ls
$ whoami
$ date
```

### Using Built-in Commands
```sh
$ help
$ mkdir test_folder
$ cd test_folder
$ !!
```

### Input & Output Redirection
```sh
$ echo "Hello" > output.txt
$ cat < output.txt
```

### Pipes
```sh
$ ls | wc -l
```

### Special Features
```sh
$ cal feb 2025 > t.txt ECHO  # Output: "cal feb 2025"
$ cat < input.txt IO  # Output: "LT input.txt"
$ cat shell.c | wc PIPE  # Output: "PIPE wc"
```

## Known Issues
- The shell does not yet support complex piping with multiple redirections.
- Background process execution (`&`) is not implemented.

## Future Improvements
- Implement signal handling (Ctrl+C to terminate processes cleanly).
- Add support for background processes.
- Improve error handling for invalid commands.

## Author
- Developed as part of a Unix shell programming assignment.


