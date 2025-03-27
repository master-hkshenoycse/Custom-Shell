#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     // For fork(), execvp()
#include <sys/wait.h>   // For wait()

#define MAX_INPUT 1024
#define MAX_ARGS 64

// Signal handler to reap zombie processes
void handle_sigchld(int sig) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

// Signal handler for SIGINT (Ctrl+C)
void handle_sigint(int sig) {
    printf("\nReceived SIGINT. Use 'exit' to quit.\nmyshell> ");
    fflush(stdout);
}

// Execute piped commands
void execute_piped_commands(char *cmd) {
    int pipefd[2];
    pid_t pid;
    int fd_in = 0;

    char *command = strtok(cmd, "|");
    while (command != NULL) {
        pipe(pipefd);
        pid = fork();

        if (pid == 0) {
            dup2(fd_in, 0); // Redirect input from previous pipe
            if (strtok(NULL, "|") != NULL) {
                dup2(pipefd[1], 1); // Redirect output to next pipe
            }
            close(pipefd[0]);

            char *args[MAX_ARGS];
            int i = 0;
            args[i] = strtok(command, " \t\n");
            while (args[i] != NULL) {
                i++;
                args[i] = strtok(NULL, " \t\n");
            }
            if (execvp(args[0], args) == -1) {
                perror("execvp failed");
                exit(EXIT_FAILURE);
            }
        } else {
            wait(NULL);
            close(pipefd[1]);
            fd_in = pipefd[0]; // Save the read end of the pipe
            command = strtok(NULL, "|");
        }
    }
}


// Execute a single command
// Execute a single command with background support
void execute_command(char *cmd) {
    char *args[MAX_ARGS];
    int i = 0;
    int background = 0;

    // Tokenize the command into arguments
    args[i] = strtok(cmd, " \t\n");
    while (args[i] != NULL) {
        i++;
        args[i] = strtok(NULL, " \t\n");
    }

    if (i > 0 && strcmp(args[i - 1], "&") == 0) {
        background = 1;
        args[i - 1] = NULL;  // Remove '&' from arguments
    }

    if (args[0] == NULL) return;

    if (strcmp(args[0], "exit") == 0) {
        printf("Exiting shell...\n");
        exit(0);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
    } else if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            perror("execvp failed");
            exit(EXIT_FAILURE);
        }
    } else {
        if (!background) {
            wait(NULL);
        } else {
            printf("Started background process with PID %d\n", pid);
        }
    }
}

int main() {

    // Register signal handler for SIGCHLD
    signal(SIGCHLD, handle_sigchld);

    signal(SIGINT, handle_sigint);  // Handle Ctrl+C

    char input[MAX_INPUT];

    while (1) {
        // Print shell prompt
        printf("myshell> ");
        fflush(stdout);

        // Read input from the user
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        // Remove trailing newline (if any)
        input[strcspn(input, "\n")] = '\0';

        // Execute the command
        execute_command(input);
    }

    return 0;
}
