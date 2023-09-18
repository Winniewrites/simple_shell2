#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LINE_LENGTH 1024

/* Declare the PATH environment variable. */
extern char **environ;

/*
 * Prints the prompt.
 */
void print_prompt() {
  printf("$ ");
}

/*
 * Reads a command from the user.
 *
 * @param line The buffer to store the command in.
 */
void read_command(char *line) {
  fgets(line, MAX_LINE_LENGTH, stdin);

  /* Remove the newline character from the end of the line. */
  line[strlen(line) - 1] = '\0';
}

/*
 * Executes a command.
 *
 * @param command The command to execute.
 */
void execute_command(char *command) {
  /* Try to find the command in the PATH environment variable. */
  char *path = getenv("PATH");
  char *full_path = NULL;
  char **args = NULL;
  pid_t child_pid;

  while (path != NULL) {
    /* Create a full path to the command. */
    full_path = malloc(strlen(path) + strlen(command) + 2);
    if (full_path == NULL) {
      perror("hsh");
      exit(1);
    }

    sprintf(full_path, "%s/%s", path, command);

    /* Check if the command exists at the full path. */
    if (access(full_path, F_OK) == 0) {
      break;
    }

    /* Free the full path. */
    free(full_path);

    /* Get the next path in the PATH environment variable. */
    path = strtok(NULL, ": ");
  }

  /* If the command was not found, print an error message and return. */
  if (full_path == NULL) {
    printf("hsh: command not found: %s\n", command);
    return;
  }

  /* Fork the process and execute the command in the child process. */
  child_pid = fork();
  if (child_pid == 0) {
    /* This is the child process. */
    char **args = malloc(2 * sizeof(char *));
    args[0] = command;
    args[1] = NULL;

    execve(full_path, args, environ);

    /* If we reach here, it means that the execve() call failed. */
    perror("hsh");
    exit(1);
  } else {
    /* This is the parent process. */
    waitpid(child_pid, NULL, 0);

    /* Free the args array. */
    free(args);
  }

  /* Free the full path. */
  free(full_path);
}

/*
 * Handles the end of file condition (Ctrl+D).
 */
void handle_eof() {
  printf("\n");
  exit(0);
}

int main() {
  char line[MAX_LINE_LENGTH];

  while (1) {
    /* Print the prompt. */
    print_prompt();

    /* Read the command from the user. */
    read_command(line);

    /* If the user typed Ctrl+D, handle it. */
    if (strcmp(line, "") == 0) {
      handle_eof();
    }

    /* Execute the command. */
    execute_command(line);
  }

  return 0;
}
