#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "optprsr.h"


const char* OPT_STRING = "";
const int MIN_ARG_NB = 1;

char** parsedArgs;

int checkEnoughArgs(int argc, char* file_name) {
    if (argc < MIN_ARG_NB + 1) {
        fprintf(stderr, "%s: Not Enough arguments: Expecting at least %d.\tUsage: %s folder1 folder2/ destination \n", file_name, MIN_ARG_NB, file_name);
        errno = EINVAL;
        return EINVAL;
    }
    return 0;
}


//parse and copy parse args to parsedArgs
int parseArgs(int argc, char* argv[]) {
    // relevant args i.e. not program name
    char** args = &argv[1]; int len = argc - 1;
    tryalc(parsedArgs = calloc(len, sizeof(char*)));
    
    for (int i = 0; i < len; i++){
        parsedArgs[i] = strndup();
        
        
    }

    return EXIT_SUCCESS;
}
