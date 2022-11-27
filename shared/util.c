/**
 * @file util.c
 * @brief Utility functions mostly used for error handling
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "util.h"


void hdlOOM(void* allocReturn) {
    // here only hdl* function where we do the test for 2 reason.
    // 1. the test and the reaction is always the same (if *alloc returned null => exit with enonmem) 
    // 2. The thing to do on error is just exiting the program so the caller won't be able to do anything afterwards anyway,
    if (allocReturn == NULL) {
        // malloc returned null => exiting with error message ENOMEM
        fprintf(stderr, "%s, Cannot allocate Memory\n", strerror(errno));
        exit(ENOMEM);
    }
}

void* tryalc(void* allocReturn) {
    hdlOOM(allocReturn);
    // if we arrived here we did not exit, hence allocReturn is not null
    return allocReturn;
}


int hdlBasicErr() {
    int savedErr = errno;
    fprintf(stderr, "Unexpected error: %s\n", strerror(savedErr));
    return -1;
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
    int isbadf = savedErr == EBADF; //strerror for ebadf is very unclear
    fprintf(stderr, "Could not read from file %s : %s %s\n", filename, strerror(savedErr), isbadf ? " or is not open for reading" : "");
    if (needsClose) {
        if (close(fd)) hdlCloseErr(filename, needsExit);
    }

    if (needsExit) exit(savedErr);
    return -1;
}


int hdlReadInErr(int needsExit) {
    int savedErr = errno;
    fprintf(stderr, "Cannot read from standard input or input was empty: %s.\n", strerror(savedErr));
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
    if (lstat(path, &infos) < 0) {
        int savedErr = errno;
        fprintf(stderr, "Cannot stat %s: %s\n", path, strerror(savedErr));
        infos.st_size = -1;
    }
    return infos;
}

struct stat stat_s(const char* path) {
    struct stat infos;
    if (stat(path, &infos) < 0) {
        int savedErr = errno;
        fprintf(stderr, "Cannot stat %s: %s\n", path, strerror(savedErr));
        infos.st_size = -1;
    }
    return infos;
}

int dateCmpr(struct timespec ts2, struct timespec ts1) {
    const time_t t1 = ts1.tv_sec, t2 = ts2.tv_sec;
    const time_t elapsed = difftime(t2, t1);
    return elapsed < 0 ? -1 : (elapsed > 0);
    //? If elapsed < 0 then ts2 < ts1. Else (elapsed > 0) will return 1 Iif ts2 > ts0, and 0, if elapsed == 0. (because the only case left is elapsed == 0)
}


int strToInt(const char* str, int base, int* result) {
    char* endptr = tryalc(malloc(strlen(str)+1));
    errno = 0;
    int res = (int) strtol(str, &endptr, base);
    // From the manual : "If there were no digits at all, strtol() stores the original value of nptr in *endptr (and returns 0)"
    //   -> in this situation we want to indicate that there has been an error. Because even though res == 0, it was not the digit that was read!
    int savedErr = errno;
    if ((savedErr == EINVAL || savedErr == ERANGE) || (res == 0 && (strcmp(str, endptr) == 0)))
        printRErr("%s: Cannot convert %s to a base %d int.\n", str, base); //print message to stderr & return -1
    
    *result = res;
    return EXIT_SUCCESS;
   
}

char* strsub(char* src, int stop_idx) {
    int sublen = stop_idx;
    char* sub = NULL;
    if (sublen < 0) errno = EINVAL;
    //will copy at most 'sublen' bytes and append a \0 if necesary at the end.
    else sub = strndup(src, sublen);
    if (sub == NULL) {
        int savedErr = errno;
        fprintf(stderr, "Cannot extract substring from \"%s\": %s\n", src, strerror(savedErr));
        //If we ran out of memory we cannot do anything. The only thing to do in this situation is to just exit.
        if (savedErr == ENOMEM) exit(ENOMEM);
        return NULL;
    }

    return sub;
}