#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "shell.h"

int main(int argc, char* argv[]) {
    printf("\n");
    int err = EXIT_SUCCESS;
    Shell* shell = new_Shell();

    for (;;) {
        // Sometimes errors stays stuck on stderr which prevents programm from displaying
        // what should be in stdout correctly

        sh_prettyPrintPath(shell);

        if (sh_getAndResolveCmd(shell) < 0 ) {
            err = EXIT_FAILURE;
            fprintf(stderr, "Command could not be resolved. Please try again.\n\n");
        }
        fflush(stderr);
    }

    return err;
}
