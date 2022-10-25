/**
 * @file util.c
 * @brief Utility functions mostly used for error handling
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "util.h"

void* tryalc(void* allocReturn) {
    if (allocReturn) return allocReturn;
    //*malloc returned null => exiting with error message ENOMEM
    fprintf(stderr, "%s, Cannot allocate Memory\n", strerror(errno));
    exit(ENOMEM);
}


int hdlOpenErr(char* filename, int needsExit) {
    int savedErr = errno;
    fprintf(stderr, "Could not open file %s : %s\n", filename, strerror(savedErr));
    if (needsExit) exit(savedErr);
    return -1;
}

int hdlCloseErr(char* filename, int needsExit) {
    int savedErr = errno;
    fprintf(stderr, "Could not close file %s : %s\n", filename, strerror(savedErr));
    if (needsExit) exit(savedErr);
    return -1;
}

int hdlReadErr(char* filename, int needsExit, int needsClose, int fd) {
    int savedErr = errno;
    fprintf(stderr, "Could not read from file %s : %s\n", filename, strerror(savedErr));
    if (needsClose) {
        if (close(fd)) hdlCloseErr(filename, needsExit);
    }
    
    if (needsExit) exit(savedErr);
    return -1;
}

int hdlWriteErr(char* filename, int needsExit, int needsClose, int fd) {
    int savedErr = errno;
    fprintf(stderr, "Could not write to file %s : %s\n", filename, strerror(savedErr));
    if (needsClose) {
        if (close(fd)) hdlCloseErr(filename, needsExit);
    }
    if (needsExit) exit(savedErr);
    return -1;
}

int hdlCopyErr(char* from, char* to, int needsExit, int needsClose, int fd_from, int fd_to) {
    int savedErr = errno;
    fprintf(stderr, "Could not copy  %s to %s: %s\n", from, to, strerror(savedErr));
    if (needsClose) {
        if (close(fd_from)) hdlCloseErr(from, needsExit);
        if (close(fd_to)) hdlCloseErr(to, needsExit);
    }

    if (needsExit) exit(savedErr);
    return -1;
}
