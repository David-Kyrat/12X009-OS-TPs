#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "shell.h"

int main(int argc, char* argv[]) {
    printf("\n");
    int err = EXIT_SUCCESS;
    Shell* shell = new_Shell();
    //initSigHandlers(shell);
    for (;;) {


        sh_prettyPrintPath(shell);
        int tmp = sh_getAndResolveCmd(shell);
        if (tmp < 0 ) {
            err = EXIT_FAILURE;
            fprintf(stderr, "Command could not be resolved. Please try again.\n\n");
        }
        //sleep(0.3);
        // Sometimes errors stays stuck on stderr which prevents programm from displaying
        // what should be in stdout correctly
        fflush(stderr);
        fflush(stdout);
    }

    return err;
}
