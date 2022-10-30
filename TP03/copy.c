#include <stdio.h>
#include <stdlib.h>
#include <string.h>     //strerror
#include <errno.h>
#include <sys/stat.h>   // open
#include <sys/types.h>  // idem
#include <fcntl.h>      //  idem
#include <unistd.h>     // close
#include <time.h>

#include "copy.h"
#include "util.h"



int is_modified(char* src, char* dest) {

    // Save the data of both files
    struct stat infos_src;
    struct stat infos_dest;

    // Check if the date of the source is older than the destination or the sizes of both are different
    if (infos_src.st_mtime != infos_dest.st_mtime || infos_src.st_size != infos_dest.st_size) return 1;
    
    // Return 0 if the dates are the same and the sizes are the same
    return (infos_src.st_mtime != infos_dest.st_mtime || infos_src.st_size != infos_dest.st_size);
    
}

int copy(const char* from, const char* to) {
    int fd_from, fd_to;
    char buff[4096];  // size of a block
    ssize_t readNb;   // number of bytes read

    // lstat the source file/folder
    struct stat infos = lstat_s(from);

    // Error handling if unable to open source in read-only or the destination in write-only, create or excl (error if create but file already exists)
    if ((fd_from = open(from, O_RDONLY)) < 0) return hdlOpenErr(from, 0);

    fd_to = open(to, O_WRONLY | O_CREAT | O_EXCL, 0644);
    if ((fd_to == -1) && (EEXIST == errno)) {
        fd_to = open(from, O_WRONLY);
    }

    // Check that it's a regular file, folder or a link
    if (!(S_ISREG(infos.st_mode) || S_ISDIR(infos.st_mode) || S_ISLNK(infos.st_mode))) {
        errno = EINVAL;
        fprintf(stderr, "\"%s\": Wrong file type\n", from);
        return -1;
    }
    

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
        hdlReadErr(from, 0, 0, (int)NULL);
        return hdlCopyErr(from, to, 1, 0, fd_from, fd_to);
    }
    if (close(fd_to) < 0) {
        close(fd_from);           // we already have a closing error that we do not now how to handle, 
        return hdlCloseErr(to, 0);// so if we have another we cannot do much more than what we're already doing
    }
    if (close(fd_from) < 0) return hdlCloseErr(from, 0);

    return EXIT_SUCCESS;
}

/* int main(int argc, char* argv[]) {
    if (copy(argv[1], argv[2])) return -1;

    return EXIT_SUCCESS;
} */
