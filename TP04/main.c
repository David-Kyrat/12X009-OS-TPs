#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "files.h"
#include "inp.h"
#include "optprsr.h"
#include "util.h"

//TODO: recomment correctly optparser.h

int main(int argc, char* argv[]) {
    const char* file = parseArgs(argc, argv);
    if (file == NULL) {
        int savedErr = errno;
        //if error message was not added to stderr
        if (savedErr != -1) fprintf(stderr, "%s\n", strerror(savedErr));
        return EXIT_FAILURE;
    }

    printf("file is %s\n", file);
    Inp* inp = NULL;

    do {
        inp = inp_askUser();
        printf("--------\n\n");
    } while (inp == NULL || 1);

    inp_print(inp);
    return EXIT_SUCCESS;
}
