#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     // For fork(), execvp()
#include <sys/wait.h>   // For wait()

#define MAX_INPUT 1024
#define MAX_ARGS 64

// Execute a single command
void execute_command(char *cmd) {
    char *args[MAX_ARGS];
    int i = 0;

    // Tokenize the command into arguments
    args[i] = strtok(cmd, " \t\n");
    while (args[i] != NULL) {
        i++;
        args[i] = strtok(NULL, " \t\n");
    }

    if (args[0] == NULL) return;  // Empty command

    // Exit command to terminate the shell
    if (strcmp(args[0], "exit") == 0) {
        printf("Exiting shell...\n");
        exit(0);
    }

    // Fork and execute the command
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
    } else if (pid == 0) {
        // Child process: execute the command
        if (execvp(args[0], args) == -1) {
            perror("execvp failed");
            exit(EXIT_FAILURE);
        }
    } else {
        // Parent process: wait for the child to complete
        wait(NULL);
    }
}

int main() {
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
