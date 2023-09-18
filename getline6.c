#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

static char *buffer = NULL;
static size_t buffer_size = 0;

char *my_getline() {
  /* Reset the buffer size */
  buffer_size = 0;

  /* Read characters from the user input until a newline character is encountered */
  while (1) {
    char c = getchar();

    /* If the end of the file is reached, return NULL */
    if (c == EOF) {
      return NULL;
    }

    /* If the buffer is full, reallocate it to twice its size */
    if (buffer_size >= BUFFER_SIZE) {
      buffer = realloc(buffer, buffer_size * 2);
      if (buffer == NULL) {
        perror("my_getline");
        exit(1);
      }
    }

    /* Store the character in the buffer */
    buffer[buffer_size++] = c;

    /* If a newline character is encountered, break out of the loop */
    if (c == '\n') {
      break;
    }
  }

  /* Add a null terminator to the end of the string */
  buffer[buffer_size++] = '\0';
  return buffer;
}

int main() {

  char *command = NULL;
  char *full_path = NULL;
  char *path = NULL;
  pid_t child_pid;

  /* Print the shell prompt */
  printf("$ ");

  /* Read the command from the user */
  command = my_getline();

  /* If the end of the file is reached, exit the program */
  if (command == NULL) {
    printf("\n");
    exit(0);
  }

  /* Remove the newline character from the end of the command */
  command[strlen(command) - 1] = '\0';

  /* Try to find the command in the PATH environment variable */
  path = getenv("PATH");

  /* If the PATH environment variable is not set, print an error message and return */
  if (path == NULL) {
    printf("hsh: PATH environment variable is not set\n");
    return 1;
  }

  /* Allocate memory for the full path to the command */
  full_path = malloc(strlen(command) + strlen(path) + 2);
  if (full_path == NULL) {
    perror("main");
    exit(1);
  }

  /* Create a full path to the command */
  sprintf(full_path, "%s/%s", path, command);

  /* Check if the command exists */
  if (access(full_path, F_OK) != 0) {
    printf("Command not found: %s\n", command);
    free(full_path);
    return 1;
  }

  /* Fork the process and execute the command in the child process */
  child_pid = fork();
  if (child_pid == 0) {
    /* Execute the command */
    char *args[2];
    args[0] = command;
    args[1] = NULL;
    execve(full_path, args, environ);
    perror("main");
    exit(1);
  } else {
    /* Wait for the child process to finish executing */
    waitpid(child_pid, NULL, 0);
  }

  /* Free the memory allocated for the full path */
  free(full_path);
  printf("$ ");

  /* Repeat the loop until the end of the file is reached */
  return 0;
}
