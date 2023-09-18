#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

char *command;
size_t command_size = 0;

char *my_getline() {
  /* Reset the buffer size */
  command_size = 0;

  while (1) {
    char c = getchar();

    if (c == EOF) {
      return NULL;
    }

    if (command_size >= BUFFER_SIZE) {
      char *new_buffer = realloc(command, command_size * 2);
      if (new_buffer == NULL) {
        perror("my_getline");
        exit(1);
      }
      command = new_buffer;
    }

    command[command_size++] = c;
    if (c == '\n') {
      break;
    }
  }

  command[command_size++] = '\0';
  return command;
}

void execute_exit_builtin(int status) {
  exit(status);
}

int main() {
  /* Declare the command variable at the beginning of the function */
  char *command;
  pid_t child_pid;

  printf("$ ");
  command = my_getline();
  if (command == NULL) {
    printf("\n");
    exit(0);
  }

  /* Remove the newline character from the end of the command and Check if the command is the `exit` built-in */
  command[strlen(command) - 1] = '\0';
  if (strcmp(command, "exit") == 0) {
    int status = 0;
    if (sscanf(command, "exit %d", &status) == 1) {
      execute_exit_builtin(status);
    } else {
      execute_exit_builtin(0);
    }
  } else {
    char *full_path = malloc(strlen(command) + 2);
    if (full_path == NULL) {
      perror("main");
      exit(1);
    }

    sprintf(full_path, "./%s", command);
    if (access(full_path, F_OK) != 0) {
      printf("Command not found: %s\n", command);
      free(full_path);
      exit(1);
    }

    child_pid = fork();
    if (child_pid == 0) {
      char *args[] = {NULL};
      /* Pass an empty array of arguments to the execve() function */
      execve(full_path, args, environ);
      perror("main");
      exit(1);
    } else {
      waitpid(child_pid, NULL, 0);
    }

    free(full_path);
  }

  printf("$ ");
  return 0;
}

