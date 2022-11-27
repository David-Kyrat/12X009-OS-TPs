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
        prettyPrintPath();
        /* const char* inp = readParseIn();
        

        if (inp != NULL) printf("%s\n", inp);
        else {
            err = EXIT_FAILURE;
            fprintf(stderr, "Please try again.\n\n");
        } */
    }

    return err;
}
