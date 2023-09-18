#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LINE_LENGTH 1024

void print_prompt() {
  printf("$ ");
}

void read_command(char *line) {
  fgets(line, MAX_LINE_LENGTH, stdin);

  /* Remove the newline character from the end of the line. */
  line[strlen(line) - 1] = '\0';
}

void execute_command(char *command) {
  char **argv = malloc(2 * sizeof(char *));
  argv[0] = command;
  argv[1] = NULL;

  execve(command, argv, environ);

  /* If we reach here, it means that the execve() call failed.*/
  perror("hsh");
  exit(1);

  free(argv);
}

void handle_eof() {
  printf("\n");
  exit(0);
}

int main() {
  char line[MAX_LINE_LENGTH];

  while (1) {
    /* Print the prompt */ 
    print_prompt();

    /* Read the command from the user */
    read_command(line);

    /*If the user typed Ctrl+D, handle it */ 
    if (strcmp(line, "") == 0) {
      handle_eof();
    }

    /* Execute the command */
    execute_command(line);
  }

  return 0;
}
