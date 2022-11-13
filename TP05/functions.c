#include "functions.h"

#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "optprsr.h"
#include "util.h"

int isPortValid(int port) {
    if (port < MIN_PORT || port > MAX_PORT) {
        errno = EINVAL;
        printErr("%s, %d: Invalid port. Expected number in [1024, 65535]\n", port);
        return 0;
    }
    return 1;
}


in_addr new_in_addr(const char* addr_repr) {
    

}

