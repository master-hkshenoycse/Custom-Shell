#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

#define HISTORY_SIZE 10

char history[HISTORY_SIZE][100];
int history_count = 0;

// Parse input into arguments
void parse_input(char *input, char **args) {
    int i = 0;
    char *token = strtok(input, " \t\n");
    while (token != NULL) {
        args[i++] = token;
        token = strtok(NULL, " \t\n");
    }
    args[i] = NULL;
}

// Trim whitespace from a string
char *trim_whitespace(char *str) {
    while (*str == ' ' || *str == '\t' || *str == '\n') str++;
    return str;
}

// Add a command to history
void add_to_history(char *input) {
    if (history_count < HISTORY_SIZE) {
        strcpy(history[history_count++], input);
    } else {
        for (int i = 1; i < HISTORY_SIZE; i++) {
            strcpy(history[i - 1], history[i]);
        }
        strcpy(history[HISTORY_SIZE - 1], input);
    }
}

// Print command history
void print_history() {
    for (int i = 0; i < history_count; i++) {
        printf("%d: %s\n", i + 1, history[i]);
    }
}
