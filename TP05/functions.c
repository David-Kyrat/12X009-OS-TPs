#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "functions.h"
#include "optprsr.h"
#include "util.h"


/**
 * Checks if the given port number is valid (i.e. between optprsr.MIN_PORT and optprsr.MAX_PORT, (inclusive))
 * @param port The port number to check.
 * @return -1 if port is invalid 0 if it is
 */
int isPortValid(int port) {
    //check_hdlError(port < MIN_PORT || port > MAX_PORT, "%s: %d, Invalid port number.\n", strerror(errno), port);
    if (port < MIN_PORT || port > MAX_PORT) {
        errno = EINVAL;
        printRErr("%s, %d: Invalid port number.\n", port);
    }
    return EXIT_SUCCESS;
}

int hdlWrongPortErr(int port) {
    errno = EINVAL;
    fprintf(stderr, "%s: %d, Invalid port number.\n", strerror(EINVAL), port);
    return 0;
}

