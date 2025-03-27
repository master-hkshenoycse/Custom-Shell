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

#define MAX_HISTORY 10
char *history[MAX_HISTORY];
int history_count = 0;

void add_to_history(char *cmd) {
    if (history_count < MAX_HISTORY) {
        history[history_count++] = strdup(cmd);
    } else {
        free(history[0]);
        for (int i = 1; i < MAX_HISTORY; i++) {
            history[i - 1] = history[i];
        }
        history[MAX_HISTORY - 1] = strdup(cmd);
    }
}

void show_history() {
    for (int i = 0; i < history_count; i++) {
        printf("%d: %s\n", i + 1, history[i]);
    }
}

void execute_builtin(char *cmd) {
    if (strcmp(cmd, "exit") == 0) {
        printf("Exiting shell...\n");
        exit(0);
    } else if (strcmp(cmd, "pwd") == 0) {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        } else {
            perror("getcwd failed");
        }
    } else if (strncmp(cmd, "cd", 2) == 0) {
        char *dir = strtok(cmd + 3, " \t\n");
        if (dir == NULL) dir = getenv("HOME");
        if (chdir(dir) != 0) {
            perror("chdir failed");
        }
    } else {
        execute_redirection(cmd);
    }
}


// Function to execute a command with redirection
void execute_redirection(char *cmd) {
    char *args[MAX_ARGS];
    int i = 0;
    int in_fd = -1, out_fd = -1, append = 0;

    // Check for redirection symbols
    char *input_file = strstr(cmd, "<");
    char *output_file = strstr(cmd, ">");
    
    if (input_file) {
        *input_file = '\0'; // Split command from input redirection
        input_file = strtok(input_file + 1, " \t\n");
    }
    if (output_file) {
        append = (*(output_file + 1) == '>') ? 1 : 0;
        *output_file = '\0'; // Split command from output redirection
        output_file = strtok(output_file + 1 + append, " \t\n");
    }

    // Tokenize command into arguments
    args[i] = strtok(cmd, " \t\n");
    while (args[i] != NULL) {
        i++;
        args[i] = strtok(NULL, " \t\n");
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
    } else if (pid == 0) {
        if (input_file) {
            in_fd = open(input_file, O_RDONLY);
            if (in_fd < 0) {
                perror("Error opening input file");
                exit(1);
            }
            dup2(in_fd, STDIN_FILENO);
            close(in_fd);
        }
        if (output_file) {
            out_fd = open(output_file, O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC), 0644);
            if (out_fd < 0) {
                perror("Error opening output file");
                exit(1);
            }
            dup2(out_fd, STDOUT_FILENO);
            close(out_fd);
        }

        if (execvp(args[0], args) == -1) {
            perror("execvp failed");
            exit(EXIT_FAILURE);
        }
    } else {
        wait(NULL);
    }
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
        printf("myshell> ");
        fflush(stdout);
        
        if (fgets(input, sizeof(input), stdin) == NULL) break;
        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "history") == 0) {
            show_history();
            continue;
        }

        add_to_history(input);

        if (strchr(input, '|')) {
            execute_piped_commands(input);
        } else {
            execute_builtin(input);
        }
    }

    return 0;
}
