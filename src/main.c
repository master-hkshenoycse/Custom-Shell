#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "execute.h"
#include "utils.h"

#define MAX_INPUT_SIZE 1024

void handle_sigint(int sig) {
    printf("\nCaught SIGINT (Ctrl+C). Type 'exit' to quit.\nmyshell> ");
    fflush(stdout);
}

int main() {
    char input[MAX_INPUT_SIZE];

    // Handle Ctrl+C (SIGINT)
    signal(SIGINT, handle_sigint);

    while (1) {
        printf("myshell> ");
        fflush(stdout);

        if (!fgets(input, MAX_INPUT_SIZE, stdin)) {
            perror("Error reading input");
            break;
        }

        // Remove newline character
        input[strcspn(input, "\n")] = 0;

        // Ignore empty input
        if (strlen(input) == 0) {
            continue;
        }

        // Add command to history
        add_to_history(input);

        // Check for built-in commands
        if (is_builtin(input)) {
            char *args[10];
            parse_input(input, args);
            execute_builtin(args);
            continue;
        }

        // Handle background processes
        int background = 0;
        if (input[strlen(input) - 1] == '&') {
            background = 1;
            input[strlen(input) - 1] = '\0'; // Remove '&'
        }

        // Handle pipes
        if (strchr(input, '|')) {
            execute_piped_commands(input);
            continue;
        }

        // Handle redirection
        if (strchr(input, '>')) {
            execute_redirection(input);
            continue;
        }

        // Execute regular command
        char *args[10];
        parse_input(input, args);
        execute_command(args, background);
    }

    return 0;
}
