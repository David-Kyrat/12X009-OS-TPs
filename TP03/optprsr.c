#include <errno.h>
#include <limits.h>  //PATH_MAX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "optprsr.h"
#include "util.h"

const char* OPT_STRING = "";
const int MIN_ARG_NB = 1;

int checkEnoughArgs(int argc, char* file_name) {
    if (argc < MIN_ARG_NB + 1) {
        fprintf(stderr, "%s: Not Enough arguments: Expecting at least %d.\n", file_name, MIN_ARG_NB);
        errno = EINVAL;
        return EINVAL;
    }
    return 0;
}

//parse and copy parse args to parsedArgs

/**
 * Parses arguments given at main function and return a copy of them.
 * 
 * @param argc the number of arguments passed to the program
 * @param argv the array of arguments passed to the program
 * 
 * @return Copy of parsed arguments.
 */
char** parseArgs(int argc, char* argv[]) {
    char** parsedArgs = NULL;
    if (checkEnoughArgs(argc, argv[0]) != 0) { errno = EINVAL; return parsedArgs; }
    
    // relevant args i.e. not program name
    char** args = &argv[1];
    int len = argc - 1;
    tryalc(parsedArgs = calloc(len, sizeof(char*)));

    for (int i = 0; i < len; i++) {
        int len = strlen(args[i]);
        if (len <= 0 || len >= PATH_MAX) { errno = EINVAL; return parsedArgs; }
        // if a given filepath is too long => return error
        parsedArgs[i] = strndup(args[i], len + 1);
    }

    return parsedArgs;
}
