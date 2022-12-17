#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

int main(int argc, char** argv) {
  char* input;
  while ((input = readline("> ")) != NULL) {
    // Do something with the input
    printf("You entered: %s\n", input);
    add_history(input);
    free(input);
  }
  return 0;
}
