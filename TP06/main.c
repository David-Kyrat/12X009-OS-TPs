#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "input.h"
#include "shell.h"


int main(int argc, char* argv[]) {

    int err = EXIT_SUCCESS;

    //init shell, todo: remove this after when proper struct was implemented
    sh_init();

    for (;;) {
        sh_prettyPrintPath();

        if (sh_getAndResolveCmd() < 0 ) {
            err = EXIT_FAILURE;
            fprintf(stderr, "Command could not be resolved. Please try again.\n\n");
        }
    }

    return err;
}
