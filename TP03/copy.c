#include <dirent.h>
#include <errno.h>
#include <fcntl.h>   //  idem
#include <limits.h>  //PATH_MAX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>     //strerror
#include <sys/stat.h>   // open
#include <sys/types.h>  // idem
#include <unistd.h>     // close

#include "copy.h"
#include "util.h"

int copy(const char* from, const char* to) {
    int fd_from, fd_to;
    char buff[4096];  // size of a block
    ssize_t readNb;   // number of bytes read

    if ((fd_from = open(from, O_RDONLY)) < 0) return hdlOpenErr(from, 0);
    if ((fd_to = open(to, O_WRONLY | O_CREAT | O_EXCL, 0600) < 0)) return hdlOpenErr(to, 0);

    //* While there are bytes left to be read, reads them 4096 by 4096 (4096 or current size of 'buff' if its not that)
    //* nb: if readNb is < to what we expected we dont really care because the program will retry until having read everything
    while ((readNb = read(fd_from, buff, sizeof buff)) > 0) {
        char* crtToCopy = buff;  // pointer to start of current 'portion of data' to copy
        ssize_t writtenNb = 0;
        do {
            writtenNb = write(fd_to, buff, readNb);
            if (writtenNb < 0 && errno != EINTR) return hdlCopyErr(from, to, 0, 1, fd_from, fd_to);
            // If serious writing error (other than just the call getting interrupted) happened => stop copying and return -1

            readNb -= writtenNb;
            crtToCopy += writtenNb;  // advance buffer section to copy
        } while (readNb > 0);
    }
    if (readNb < 0) {
        hdlReadErr(from, 0, 0, NULL);
        return hdlCopyErr(from, to, 1, 0, fd_from, fd_to);
    }
    if (close(fd_to) < 0) {
        close(fd_from);           // we already have a closing error that we do not now how to handle, 
        return hdlCloseErr(to, 0);// so if we have another we cannot do much more than what we're already doing
    }
    if (close(fd_from) < 0) return hdlCloseErr(from, 0);

    return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {
    if (copy(argv[1], argv[2])) return -1;

    return EXIT_SUCCESS;
}