/* Simple shell */
#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Declare the built-in function prototypes */
void execute_exit_builtin();
void execute_env_builtin();

void execute_command(char *command) {
  /* Declare the path, full_path, and child_pid variables */
  char *path;
  char *full_path;
  pid_t child_pid;

  /* Check if the command is the exit built-in */
  if (strcmp(command, "exit") == 0) {
    execute_exit_builtin();
    return;
  }

  /* Check if the command is the env built-in */
  if (strcmp(command, "env") == 0) {
    execute_env_builtin();
    return;
  }

  /* Try to find the command in the PATH environment variable */
  path = getenv("PATH");

  /* If the PATH environment variable is not set, print an error message and return */
  if (path == NULL) {
    printf("hsh: PATH environment variable is not set\n");
    return;
  }

  /* Allocate memory for the full path to the command */
  full_path = malloc(strlen(path) + strlen(command) + 2);
  if (full_path == NULL) {
    perror("hsh");
    exit(1);
  }

  /* Create a full path to the command */
  sprintf(full_path, "%s/%s", path, command);

  /* Check if the command exists at the full path */
  if (access(full_path, F_OK) != 0) {
    printf("hsh: command not found: %s\n", command);
    free(full_path);
    return;
  }

  /* Fork the process and execute the command in the child process */
  child_pid = fork();
  if (child_pid == 0) {
    /* Execute the command */
    char *args[2];
    args[0] = command;
    args[1] = NULL;
    execve(full_path, args, environ);

    /* If the execve() call failed */
    perror("hsh");
    exit(1);
  } else {
    /* Wait for the child process to finish executing */
    waitpid(child_pid, NULL, 0);
  }

  /* Free the memory allocated for the full path */
  free(full_path);
}
