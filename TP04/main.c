#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "util.h"
#include "optprsr.h"
#include "files.h"


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

    return EXIT_SUCCESS;
}
