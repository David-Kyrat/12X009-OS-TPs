#include <errno.h>
#include <getopt.h>
#include <limits.h>  //PATH_MAX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "files.h"
#include "optprsr.h"
#include "util.h"

static const char VALID_CMD[3] = {'g', 's', 'w'}, VALID_LTYPE[3] = {'r', 'w', 'u'}, VALID_WHENCE[3] = {'s', 'c', 'e'};
static const char HELP_CHAR = '?', QUIT_CHAR = 'Q';

//const char* OPT_STRING = "fa";
const int MIN_ARG_NB = 1, MAX_ARG_NB = 1;
const char* USAGE_MSG = "Usage: %s folder1 folder2/ destination \n";

/**
 * @brief Check if there is at least 1 argument.
 * @param argc same as argc in main
 * @return 0 if success otherwise code 22 (Invalid argument)
 */
int checkArgsNb(int argc) {
    int relevantNb = argc - 1;
    if (relevantNb < MIN_ARG_NB || relevantNb > MAX_ARG_NB) {
        errno = EINVAL;
        return EINVAL;
    }
    return 0;
}

const char* parseArgs(int argc, char* argv[]) {
    const char* parsedArg;
    if (checkArgsNb(argc) != 0) {
        fprintf(stderr, "%s: Not Enough arguments: Expecting at least %d.\n", argv[0], MIN_ARG_NB);
        return NULL;
    }

    // relevant arg i.e. not program name. (Waiting only 1 arg)
    char* arg = argv[1];

    int argLen = strlen(arg);
    if (argLen <= 0 || argLen >= PATH_MAX) {
        errno = EINVAL;
        return NULL;
    }
    // argv[i] is not destination i.e. last element and argv[i] does not exists
    if (!exists(arg)) {
        errno = ENOENT;
        fprintf(stderr, "\"%s\" Invalid argument: %s", arg, strerror(ENOENT));
        return NULL;
    }
    //returns absolute path if it was computed correctly.
    parsedArg = realpath(arg, NULL);
    if (parsedArg == NULL) {
        int savedErr = errno;
        fprintf(stderr, "Cannot resolve file %s: %s\n", arg, strerror(savedErr));
        return NULL;
    }

    return parsedArg;
}

int parseInput(const char* INP_FORMAT, char* cmd, char* ltype, int* start, int* stop, char* whence) {
    char* buf = calloc(MAX_INPUT, sizeof(char));
    fgets(buf, MAX_INPUT, stdin);
    int an = sscanf(buf, INP_FORMAT, cmd, ltype, start, stop, whence);
    int isWhenceGiven = 0;

    switch (an) {
        case 4:
            printf("whence not given\n");
            break;
        case 5:
            isWhenceGiven = 1;
            printf("whence given\n");
            break;

        case 1:
            //*if user did not entered '?' or 'quit_char' then having only 1 parameter is not valid.
            //* then the program will then not enter the if clause, i.e. => not break => continue to default branch => finally return -1 with errno set to EINVAL.
            if (*cmd == HELP_CHAR) {
                //if user seeked for help (entered ?) => returns 2 to indicate that. else does what was described above.
                free(buf);
                return 2;
            } else if (*cmd == QUIT_CHAR) {
                printf("%c was pressed, exiting...\n", QUIT_CHAR);
                exit(EXIT_SUCCESS);
            }

        default:
            errno = EINVAL;
            fprintf(stderr, "%s: Expecting between 4 and 5 inputs\n", strerror(EINVAL));
            free(buf);
            return -1;
            break;
    }

    //* checks if each char paramter is valid i.e. if each belongs to the corresponding array 'VALID_<param_name>' in optprsr.c
    int match = 3;
    //* if match attain 0 then one of the param is not any of the declared valid values => hence our input is invalid.
    for (int i = 0; i < sizeof VALID_CMD; i++) {
        if (*cmd != VALID_CMD[i]) match--;
    }
    if (match > 0) {
        match = 3;
        for (int i = 0; i < sizeof VALID_LTYPE; i++) {
            if (*ltype != VALID_LTYPE[i]) match--;
        }
    }
    if (match > 0 && isWhenceGiven) {
        match = 3;
        for (int i = 0; i < sizeof VALID_WHENCE; i++) {
            if (*whence != VALID_WHENCE[i]) match--;
        }
    }
    if (match <= 0) {
        errno = EINVAL;
        free(buf);
        return -1;
    }

    free(buf);
    return EXIT_SUCCESS;
}
