#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "input.h"
#include "shellstruct.h"

int main(int argc, char* argv[]) {

    int err = EXIT_SUCCESS;
    Shell* shell = new_Shell();

    for (;;) {
        sh_prettyPrintPath(shell);

        if (sh_getAndResolveCmd(shell) < 0 ) {
            err = EXIT_FAILURE;
            fprintf(stderr, "Command could not be resolved. Please try again.\n\n");
        }
    }

    return err;
}
