#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "files.h"
#include "inp.h"
#include "lock.h"
#include "optprsr.h"
#include "util.h"


int main(int argc, char* argv[]) {
    const char* file = parseArgv(argc, argv);
    if (file == NULL) {
        int savedErr = errno;
        //if error message was not added to stderr
        if (savedErr != -1) fprintf(stderr, "%s\n", strerror(savedErr));
        return EXIT_FAILURE;
    }
    printf("\nOperating on file: \"%s\"\n", file);
    int fd = open(file, O_RDWR);
    if (fd < 0) return hdlOpenErr(file, 1); // hanlde error and exits if we cannot open file

    Inp* inp = NULL;
    int err = EXIT_SUCCESS;
    for(;;) {
        free(inp);
        inp = inp_askUser();
        if (inp != NULL) {
            err = lock(fd, inp);
        }
        printf("--------\n\n");
    }
    
    free(inp);
    free(file);
    return err != 0;
}
