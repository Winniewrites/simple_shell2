#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

static char *buffer = NULL;
static size_t buffer_size = BUFFER_SIZE;

char *my_getline() {
  size_t pos = 0;
  int c;

  /* Allocate memory for the buffer */
  char *new_buffer = (char *)malloc(buffer_size);
  if (new_buffer == NULL) {
    perror("my_getline");
    exit(1);
  }

  /* Read characters from the user input until a newline character is encountered */
  while (1) {
    c = getchar();

    /* If the end of the file is reached or a newline character is encountered, terminate the string */
    if (c == EOF || c == '\n') {
      buffer[pos] = '\0';
      break;
    }

    /* If the buffer is full, reallocate it to twice its size */
    if (pos >= buffer_size - 1) {
      buffer_size *= 2;
      new_buffer = (char *)realloc(buffer, buffer_size);
      if (new_buffer == NULL) {
        perror("my_getline");
        exit(1);
      }
      buffer = new_buffer;
    }

    /* Store the character in the buffer */
    buffer[pos] = (char)c;
    pos++;
  }

  return buffer;
}


void execute_exit_builtin() {
  exit(0);
}

void execute_env_builtin() {
  /* Print the current environment */
  char **env = environ;
  while (*env != NULL) {
    printf("%s\n", *env);
    env++;
  }
}

void execute_setenv_builtin(const char *variable, const char *value) {
  /* Set or modify the environment variable */
  setenv(variable, value, 1);
}

void execute_unsetenv_builtin(const char *variable) {
  unsetenv(variable);
}

int main() {
  pid_t child_pid;
  char *command;
  char *full_path;

  printf("$ ");

  while (1) {
    command = my_getline();
    if (command == NULL) {
      printf("\n");
      exit(0);
    }

    /* Remove the newline character from the end of the command */
    command[strlen(command) - 1] = '\0';

    /* Check if the command is a built-in command */
    if (strcmp(command, "exit") == 0) {
      execute_exit_builtin();
    } else if (strcmp(command, "env") == 0) {
      execute_env_builtin();
    } else if (strncmp(command, "setenv", 6) == 0) {
      char variable[BUFFER_SIZE];
      char value[BUFFER_SIZE];
      sscanf(command + 6, "%s %s", variable, value);

      execute_setenv_builtin(variable, value);
    } else if (strncmp(command, "unsetenv", 7) == 0) {
      char variable[BUFFER_SIZE];
      sscanf(command + 7, "%s", variable);

      execute_unsetenv_builtin(variable);
    } else {
      full_path = (char *)malloc(strlen(command) + 3);
      if (full_path == NULL) {
        perror("main");
        exit(1);
      }
      sprintf(full_path, "./%s", command);

      /* Check if the command exists */
      if (access(full_path, F_OK) != 0) {
        printf("Command not found: %s\n", command);
        free(full_path);
        continue;
      }

      child_pid = fork();
      if (child_pid == 0) {
        /* Pass an empty array of arguments to the execve() function */
        char *args[2];
        args[0] = full_path;
        args[1] = NULL;
        execve(full_path, args, environ);
        perror("main");
        exit(1);
      } else {
        waitpid(child_pid, NULL, 0);
        free(full_path);
      }
    }

    printf("$ ");
  }

  free(buffer);

  return 0;
}
