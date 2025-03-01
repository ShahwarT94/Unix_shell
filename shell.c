#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>  


#define MAX_LINE 1024
#define MAX_ARGS 64

char last_command[MAX_LINE] = ""; // Store last command for '!!'

// Parse command into tokens
int parse_command(char *line, char **args) {
    int argc = 0;
    char *token = strtok(line, " \t\n");  // <-- Ensure quotes are correctly placed
    while (token != NULL && argc < MAX_ARGS - 1) {
        args[argc++] = token;
        token = strtok(NULL, " \t\n");  // <-- Fix here too
    }
    args[argc] = NULL;
    return argc;
}


// Handle built-in commands
int handle_builtin(char **args, int argc) {
    if (strcmp(args[0], "exit") == 0) {
        exit(0);
    } else if (strcmp(args[0], "help") == 0) {
        printf("Available commands:\n");
        printf(" help - Show this help message\n");
        printf(" exit - Exit the shell\n");
        printf(" cd <dir> - Change directory\n");
        printf(" mkdir <dir> - Create directory\n");
        printf(" !! - Repeat last command\n");
        return 1;
    } else if (strcmp(args[0], "cd") == 0) {
        if (argc < 2) {
            fprintf(stderr, "cd: missing argument\n");
        } else {
            if (chdir(args[1]) != 0) {
                perror("cd");
            }
        }
        return 1;
    } else if (strcmp(args[0], "mkdir") == 0) {
        if (argc < 2) {
            fprintf(stderr, "mkdir: missing argument\n");
        } else {
            if (mkdir(args[1], 0755) != 0) {
                perror("mkdir");
            }
        }
        return 1;
    }
    return 0;
}


// Execute a command with optional redirection
void execute_command(char **args, int argc) {
    int redirect_in = 0, redirect_out = 0;
    char *infile = NULL, *outfile = NULL;

    // Check for redirection
    for (int i = 0; i < argc; i++) {
        if (strcmp(args[i], ">") == 0) {
            if (i + 1 < argc) {
                redirect_out = 1;
                outfile = args[i + 1];
                args[i] = NULL;
            }
        } else if (strcmp(args[i], "<") == 0) {
            if (i + 1 < argc) {
                redirect_in = 1;
                infile = args[i + 1];
                args[i] = NULL;
            }
        }
    }

    // Handle built-in commands
    if (handle_builtin(args, argc)) {
        return;
    }

    pid_t pid = fork();
    if (pid == 0) { // Child process
        if (redirect_out) {
            int fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) {
                perror("open");
                exit(1);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        if (redirect_in) {
            int fd = open(infile, O_RDONLY);
            if (fd < 0) {
                perror("open");
                exit(1);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
        }
        execvp(args[0], args);
        perror("execvp");
        exit(1);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
    } else {
        perror("fork");
    }
}

// Execute commands with pipes
void execute_pipes(char *line) {
    char *cmd1 = strtok(line, "|");
    char *cmd2 = strtok(NULL, "|");
    
    if (!cmd2) {
        fprintf(stderr, "Invalid pipe command\n");
        return;
    }
    
    char *args1[MAX_ARGS], *args2[MAX_ARGS];
    parse_command(cmd1, args1);
    parse_command(cmd2, args2);
    
    int pipe_fd[2];
    pipe(pipe_fd);
    
    pid_t pid1 = fork();
    if (pid1 == 0) { // First command
        close(pipe_fd[0]);
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[1]);
        execvp(args1[0], args1);
        perror("execvp");
        exit(1);
    }
    
    pid_t pid2 = fork();
    if (pid2 == 0) { // Second command
        close(pipe_fd[1]);
        dup2(pipe_fd[0], STDIN_FILENO);
        close(pipe_fd[0]);
        execvp(args2[0], args2);
        perror("execvp");
        exit(1);
    }
    
    close(pipe_fd[0]);
    close(pipe_fd[1]);
    wait(NULL);
    wait(NULL);
}

int main() {
    char line[MAX_LINE];
    char *args[MAX_ARGS];
    
    while (1) {
        printf("$ ");
        fflush(stdout);
        if (fgets(line, MAX_LINE, stdin) == NULL) {
            printf("\nGoodbye!\n");
            break;
        }
        line[strcspn(line, "\n")] = 0; // Remove newline
        if (strlen(line) == 0) {
            continue;
        }
        strcpy(last_command, line);
        
        if (strchr(line, '|')) {
            execute_pipes(line);
        } else {
            int argc = parse_command(line, args);
            if (argc > 0) {
                execute_command(args, argc);
            }
        }
    }
    return 0;  
}
