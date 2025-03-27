#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include "execute.h"
#include "utils.h"

// Execute commands with background support
void execute_command(char **args, int background) {
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("Fork failed");
        return;
    } 
    else if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            perror("Command execution failed");
        }
        exit(EXIT_FAILURE);
    } 
    else {
        if (!background) {
            waitpid(pid, NULL, 0);
        }
    }
}

// Execute piped commands
void execute_piped_commands(char *input) {
    char *cmd1 = strtok(input, "|");
    char *cmd2 = strtok(NULL, "|");

    if (!cmd2) {
        fprintf(stderr, "Invalid pipe syntax\n");
        return;
    }

    int pipefd[2];
    pipe(pipefd);

    pid_t pid1 = fork();
    if (pid1 == 0) {
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);
        char *args1[10];
        parse_input(cmd1, args1);
        execvp(args1[0], args1);
        exit(EXIT_FAILURE);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);
        char *args2[10];
        parse_input(cmd2, args2);
        execvp(args2[0], args2);
        exit(EXIT_FAILURE);
    }

    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}

// Execute I/O redirection
void execute_redirection(char *input) {
    char *cmd = strtok(input, ">");
    char *file = strtok(NULL, ">");

    if (!file) {
        fprintf(stderr, "Invalid redirection syntax\n");
        return;
    }

    file = trim_whitespace(file);
    int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("File open failed");
        return;
    }

    pid_t pid = fork();
    if (pid == 0) {
        dup2(fd, STDOUT_FILENO);
        close(fd);
        char *args[10];
        parse_input(cmd, args);
        execvp(args[0], args);
        exit(EXIT_FAILURE);
    } else {
        close(fd);
        waitpid(pid, NULL, 0);
    }
}

// Check if the command is built-in
int is_builtin(char *cmd) {
    return (strcmp(cmd, "cd") == 0 || strcmp(cmd, "exit") == 0 || strcmp(cmd, "history") == 0);
}

// Execute built-in commands
void execute_builtin(char **args) {
    if (strcmp(args[0], "cd") == 0) {
        if (chdir(args[1]) != 0) {
            perror("cd failed");
        }
    } else if (strcmp(args[0], "exit") == 0) {
        exit(0);
    } else if (strcmp(args[0], "history") == 0) {
        print_history();
    }
}
