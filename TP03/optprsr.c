#include <errno.h>
#include <getopt.h>
#include <limits.h>  //PATH_MAX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "optprsr.h"
#include "util.h"

const char* OPT_STRING = "fa";
const int MIN_ARG_NB = 1;
const char* USAGE_MSG = "Usage: %s folder1 folder2/ destination \n";

int checkEnoughArgs(int argc, char* file_name) {
    if (argc < MIN_ARG_NB + 1) {
        //fprintf(stderr, "%s: Not Enough arguments: Expecting at least %d.\n", file_name, MIN_ARG_NB);
        errno = EINVAL;
        return EINVAL;
    }
    return 0;
}

char** parseArgs(int argc, char* argv[], int* fileNb) {
    char** parsedArgs = NULL;
    if (checkEnoughArgs(argc, argv[0]) != 0) {
        errno = EINVAL;
        return parsedArgs;
    }

    //TODO check error on parseOptArgs
    fprintf(stderr, USAGE_MSG, argv[0]);

    // relevant args i.e. not program name
    char** args = &argv[1];
    int len = argc - 1;
    tryalc(parsedArgs = calloc(len, sizeof(char*)));

    for (int i = 0; i < len; i++) {
        int argLen = strlen(args[i]);
        if (argLen <= 0 || argLen >= PATH_MAX) {
            errno = EINVAL;
            return parsedArgs;
        }
        // if a given filepath is too long => return error
        parsedArgs[i] = strndup(args[i], argLen + 1);
    }
    *fileNb = len;

    return parsedArgs;
}

int parseOptArgs(int argc, char* argv[]) {
    int state = 0, opt;

    while ((opt = getopt(argc, argv, OPT_STRING)) != -1) {
        switch (opt) {
            case 'f':
                state += 1;
                break;
            case 'a': {
                //* check if state is != 0 (in 'state ?'), i.e. if -f was passed before
                state += 2;
                break;
            }
            default:
                errno = EINVAL;
                return -1;
        }
    }
    return state;
}
