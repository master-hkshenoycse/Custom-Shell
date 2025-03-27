#ifndef EXECUTE_H
#define EXECUTE_H

void execute_command(char **args, int background);
void execute_piped_commands(char *input);
void execute_redirection(char *input);
int is_builtin(char *cmd);
void execute_builtin(char **args);

#endif
