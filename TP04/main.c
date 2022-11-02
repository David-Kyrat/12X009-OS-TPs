#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "util.h"
#include "optprsr.h"
#include "files.h"


//TODO: recomment correctly optparser.h

/**
 * basic user input can enter between 4 and 5 thing
 * -- 1. Command (1 char) ’g’ (F_GETLK), ’s’ (F_SETLK), or ’w’ (F_SETLKW)
 *
 * -- 2. Lock type (1 char) ’r’ (F_RDLCK), ’w’ (F_WRLCK), or ’u’ (F_UNLCK)
 * 
 * -- 3. start (int)
 * -- 4. stop (int)
 * 
 * -- 5. whence (1 char) 's’ (SEEK_SET , default), ’c’ (SEEK_CUR), or ’e’ (SEEK_END)
 */

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
