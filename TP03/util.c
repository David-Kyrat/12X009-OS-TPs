/**
 * @file util.c
 * @brief Utility functions mostly used for error handling
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "util.h"

void* tryalc(void* allocReturn) {
    if (allocReturn) return allocReturn;
    //*malloc returned null => exiting with error message ENOMEM
    fprintf(stderr, "%s, Cannot allocate Memory\n", strerror(errno));
    exit(ENOMEM);
}


int hdlOpenErr(const char* filename, int needsExit) {
    int savedErr = errno;
    fprintf(stderr, "Could not %s file %s : %s\n", savedErr == EEXIST ? "create" : "open", filename, strerror(savedErr));
    if (needsExit) exit(savedErr);
    return -1;
}

int hdlCloseErr(const char* filename, int needsExit) {
    int savedErr = errno;
    fprintf(stderr, "Could not close file %s : %s\n", filename, strerror(savedErr));
    if (needsExit) exit(savedErr);
    return -1;
}

int hdlReadErr(const char* filename, int needsExit, int needsClose, int fd) {
    int savedErr = errno;
    fprintf(stderr, "Could not read from file %s : %s\n", filename, strerror(savedErr));
    if (needsClose) {
        if (close(fd)) hdlCloseErr(filename, needsExit);
    }
    
    if (needsExit) exit(savedErr);
    return -1;
}

int hdlWriteErr(const char* filename, int needsExit, int needsClose, int fd) {
    int savedErr = errno;
    fprintf(stderr, "Could not write to file %s : %s\n", filename, strerror(savedErr));
    if (needsClose) {
        if (close(fd)) hdlCloseErr(filename, needsExit);
    }
    if (needsExit) exit(savedErr);
    return -1;
}

int hdlCopyErr(const char* from, const char* to, int needsExit, int needsClose, int fd_from, int fd_to) {
    int savedErr = errno;
    fprintf(stderr, "Could not copy %s to %s: %s\n", from, to, strerror(savedErr));
    if (needsClose) {
        if (close(fd_from)) hdlCloseErr(from, needsExit);
        if (close(fd_to)) hdlCloseErr(to, needsExit);
    }

    if (needsExit) exit(savedErr);
    return -1;
}

int hdlCatErr(const char* current) {
    int savedErr = errno;
    fprintf(stderr, "Cannot build path containing %s: %s\n", current, strerror(savedErr));
    return -1;
}


struct stat lstat_s(const char* path) {
    struct stat infos;
    if (lstat(path, &infos) < 0) fprintf(stderr, "Cannot stat %s: %s\n", path, strerror(errno));
    return infos;
}


struct stat stat_s(const char* path) {
    struct stat infos;
    if (stat(path, &infos) < 0) fprintf(stderr, "Cannot stat %s: %s\n", path, strerror(errno));
    return infos;
}
