#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>  //strerror
#include <dirent.h>
#include <limits.h>  //PATH_MAX
#include <sys/stat.h>  // open
#include <sys/types.h> // idem
#include <fcntl.h>    //  idem
#include <unistd.h> // close
#include "util.h"
#include "copy.h"

int copy(const char* from, const char* to) {
    int fd_from, fd_to;
    char buff[4096]; // size of a block
    ssize_t nread; // number of bytes read

    if ((fd_from = open(from, O_RDONLY)) < 0) return hdlOpenErr(from, 0);
    if ((fd_to = open(to, O_WRONLY | O_CREAT | O_EXCL, 0600) < 0)) return hdlOpenErr(to, 0);

    return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {

    if (copy(argv[1], argv[2])) return -1;

    return EXIT_SUCCESS;
}
