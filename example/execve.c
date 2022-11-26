#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[], char *env[]) {
    int i;
    for (i = 0; i < argc; i++) {
        pid_t pid;
        if ((pid = fork()) == 0) {
            char *new_argv[] = {argv[i], NULL};
            if (execve(argv[i], new_argv, env) == -1) {
                perror(argv[i]);
                exit(EXIT_FAILURE);
            }
            printf("This process was succesfully spawned");
            // WILL THE LINE ABOVE BE EXECUTED ?
        }
    }
    return 0;
}
