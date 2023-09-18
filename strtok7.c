#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

/* Dynamic array to store the command line. */
char *command;
size_t command_size = 0;

/* Dynamic array to store the full path to the command. */
char *full_path;
size_t full_path_size = 0;

/* Child process ID. */
pid_t child_pid = 0;

/* Dynamic array to store the arguments to the command. */
char **args;
size_t args_size = 0;

char *my_getline() {
  /* Read characters from the user input until a newline character is encountered */
  while (1) {
    char c = getchar();

    /* If the end of the file is reached, return NULL */
    if (c == EOF) {
      return NULL;
    }

    /* If the buffer is full, reallocate it to twice its size */
    if (command_size >= BUFFER_SIZE) {
      char *new_buffer = realloc(command, command_size * 2);
      if (new_buffer == NULL) {
        perror("my_getline");
        exit(1);
      }
      command = new_buffer;
    }

    /* Store the character in the buffer */
    command[command_size++] = c;

    /* If a newline character is encountered, break out of the loop */
    if (c == '\n') {
      break;
    }
  }

  /* Add a null terminator to the end of the command */
  command[command_size++] = '\0';

  return command;
}

int main() {
  /* Display the prompt */
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

  /* Allocate memory for the full path to the command */
  full_path_size = strlen(command) + 2;
  full_path = malloc(full_path_size);
  if (full_path == NULL) {
    perror("main");
    exit(1);
  }

  /* Create a full path to the command */
  sprintf(full_path, "./%s", command);

  /* Check if the command exists */
  if (access(full_path, F_OK) != 0) {
    printf("Command not found: %s\n", command);
    free(full_path);
    exit(1);
  }

  /* Allocate memory for the arguments to the command */
  args_size = 2;
  args = malloc(args_size * sizeof(char *));
  if (args == NULL) {
    perror("main");
    exit(1);
  }

  /* Add the full path to the command to the arguments array */
  args[0] = full_path;

  /* Add a NULL pointer to the end of the arguments array */
  args[1] = NULL;

  /* Fork the process and execute the command in the child process */
  child_pid = fork();
  if (child_pid == 0) {
    /* Execute the command */
    execve(full_path, args, environ);
    perror("main");
    exit(1);
  }

  /* Wait for the child process to terminate */
  waitpid(child_pid, NULL, 0);

  /* Free the full path to the command */
  free(full_path);

  /* Free the arguments to the command */
  free(args);

  /* Display the prompt again */
  printf("$ ");

  return 0;
}
