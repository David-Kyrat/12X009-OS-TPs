/**
 * @file util.c
 * @brief Utility functions mostly used for error handling
 */
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "util.h"


void hdlOOM(void* allocReturn) {
    // here only hdl* function where we do the test for 2 reason.
    // 1. the test and the reaction is always the same (if *alloc returned null => exit with enonmem) 
    // 2. The thing to do on error is just exiting the program so the caller won't be able to do anything afterwards anyway,
    if (allocReturn == NULL) {
        // malloc returned null => exiting with error message ENOMEM
        int err = errno;
        fprintf(stderr, "%s, Cannot allocate Memory\n", strerror(err));
        exit(ENOMEM);
    }
}


void* tryalc(void* allocReturn) {
    hdlOOM(allocReturn);
    // if we arrived here we did not exit, hence allocReturn is not null
    return allocReturn;
}


void* tryalloc(size_t size) {
    void* tmp = malloc(size);
    hdlOOM(tmp);
    memzero(tmp, size);
    return tmp;
}


void memzero(void * data, unsigned long n) { 
    memset(data, 0, n); 
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


int hdlSigHdlErr(const char* signame, int needsExit) {
    int savedErr = errno;
    fprintf(stderr, "%s: Cannot change handler for %s, exiting...\n", strerror(savedErr), signame);
    if (needsExit) exit(savedErr);
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


int strstartwith(const char* str, const char* prefix) {
    if (str == NULL || prefix == NULL) {
        errno = EINVAL;
        return -1;
    }
    return strncmp(str, prefix, strlen(prefix)) == 0;
}

char** strsplit(char* string, const char* separator, size_t* size) {
    if (string == NULL) {
        errno = EINVAL;
        return NULL;
    }
    if (separator == NULL) {
        char** wrapper = tryalc(malloc(sizeof(char*)));
        *wrapper = string;
        *size = 0;
        return wrapper;
    }
    size_t max_len = strlen(string), crt_len = 0;
    char* token;

    char** out = calloc(max_len + 1, sizeof(char*));              //array containing the split parts of string
    char* text = (char*)tryalc(strndup(string, strlen(string)));  //copy that strtok can modify at will

    token = strtok(text, separator);
    do {
        size_t token_len = strlen(token);
        out[crt_len] = tryalc(malloc(token_len + 1));
        strncpy(out[crt_len++], token, token_len + 1);

    } while ((token = strtok(NULL, separator)) != NULL);

    /* In strok each returned "split string" aren't the extracted subparts of 'text' at all.
           Instead they are just multiple pointers on the very _same_ string where the separators in it 
           have been replaced by the "terminating null byte" '\0'. Which means that the data these pointers points to completely overlap.
           I.e. From each substring, we can access and modify every other as we want very easily. (i.e. we can modify data we are not supposed to have access to)
           Hence why we are copying at max 'token_len'+1 bytes of each portion into 'out' to avoid this behavior
           and impose a real separation of each part.*/
    *size = crt_len;
    free(text);
    return out;
}


int streq(const char* s1, const char* s2) {
    if (s1 == NULL || s2 == NULL) return -2;
    return strcmp(s1, s2) == 0;
}


int strswitch(const char* pattern, const char* cases[], int caseNb) {
    if (pattern == NULL || cases == NULL) return -1;
    for (int i = 0; i < caseNb; i++) {
        if (streq(pattern, cases[i])) return i;
    }
    return -1;
}


void arrPrint(char** arr, int size) {
    printf("[");
    for (int i = 0; i < size - 1; i++)
        printf("\'%s\'#", arr[i]);
    printf("\'%s\']\n", arr[size - 1]);
}


/**
 * Remove all whitespace to the left of the given string. 
 * A whitespace is a char s such that isspace(s) returns 1. (lib ctype.h)
 * @param str string to left-strip
 * @return left-stripped string
 */
const char* stripl(char* str) {
    if (str == NULL) return NULL;
    while (isspace(*str) && *str != '\0') str++;
    return str;
}

const char* stripr(char* str) {
    if (str == NULL) return NULL;
    char* ptr = str + strlen(str) - 1;
    while (ptr && isspace(*ptr)) {
        *ptr = '\0';
        ptr--;
    }
    return str;
}

const char* strip(char* str) {
    return stripr(stripl(str));
}


/**
 * Common "private" function to avoid code repetition while handling error related to wait*() calls
 * 
 * @param pid pid of child to wait
 * @param exitStatus variable into which the exit status of child will be stored
 * @param isWaitPid != 0 value will call 'waitpid(pid, exitStatus, 0)', otherwise will call 'wait(exitStatus)'
 * @return int 
 */
int __wait_s(pid_t pid, int* exitStatus, int isWaitPid) {
    int waitCode = isWaitPid ? waitpid(pid, exitStatus, 0) : wait(exitStatus);

    if (waitCode == -1) {
        int savedErr = errno;
        if (savedErr == ECHILD) {
            /* const char msg[] = "wait_s: No child to terminate.\n";
            write(2, msg, strlen(msg) + 1); */
            return 2;

        } else if (savedErr == EINTR) {
            const char msg[] = "interrupted retrying\n";
            write(2, msg, strlen(msg) + 1);
            __wait_s(pid, exitStatus, isWaitPid);

        } else {
            fprintf(stderr, "%s: cannot kill child.\n", strerror(savedErr));
            return -1;
        }
    }

    return EXIT_SUCCESS;
}

int wait_s(int* exitStatus) {
   return __wait_s(-2, exitStatus, 0);
}

int waitpid_s(pid_t pid, int* exitStatus) {
    return __wait_s(pid, exitStatus, 1);
}

