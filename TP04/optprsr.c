#include <errno.h>
#include <getopt.h>
#include <limits.h>  //PATH_MAX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "files.h"
#include "optprsr.h"
#include "util.h"
#include "lock.h"

static const char VALID_CMD[3] = {'g', 's', 'w'}, VALID_LTYPE[3] = {'r', 'w', 'u'}, VALID_WHENCE[3] = {'s', 'c', 'e'};
static const char HELP_CHAR = '?', QUIT_CHAR = 'q';

//const char* OPT_STRING = "fa";
const int MIN_ARG_NB = 1, MAX_ARG_NB = 1;
const char* USAGE_MSG = "Usage: %s folder1 folder2/ destination \n";

//* Size of the file given when calling program
size_t FILE_SIZE; 


/**
 * @brief Check if there is at least 1 argument.
 * @param argc same as argc in main
 * @return 0 if success otherwise code 22 (Invalid argument)
 */
int checkArgsNb(int argc) {
    int relevantNb = argc - 1;
    if (relevantNb < MIN_ARG_NB || relevantNb > MAX_ARG_NB) {
        errno = EINVAL;
        exit(errno);
    }
    return 0;
}


int isLockInputValid(char* cmd, char* ltype, long start, long length, char* whence) {
    //*
    //* Checks if each char parameter is valid i.e. if each belongs to the corresponding array 'VALID_<param_name>' in optprsr.c
    //*
    long abs_start = start < 0 ? -start : start; //checking if absolute value of start, length are within FILE_SIZE. Abs val because start can be negative if, e.g., SEEK_END was provided
    long abs_length = length < 0 ? -length : length;
    if (abs_length > FILE_SIZE || abs_start >= FILE_SIZE) {
        fprintf(stderr, "Invalid start or length value. (%ld, %ld) \n", start, length);
        errno = EINVAL;
        return 0;
    }

    int match = 3;
    //* if match attain 0 then one of the param is not any of the declared valid values => hence our input is invalid.
    for (int i = 0; i < sizeof VALID_CMD; i++) 
        if (*cmd != VALID_CMD[i]) match--;
    
    if (match > 0) {
        match = 3;
        for (int i = 0; i < sizeof VALID_LTYPE; i++)
            if (*ltype != VALID_LTYPE[i]) match--;
        
    }
    if (match > 0 && whence != NULL) {
        match = 3;
        for (int i = 0; i < sizeof VALID_WHENCE; i++)
            if (*whence != VALID_WHENCE[i]) match--;
    }

    if (match <= 0) {
        errno = EINVAL;
        return -1;
    }

    return 1;
}


int parseInput(const char* INP_FORMAT, char* cmd, char* ltype, long* start, long* length, char* whence) {
    size_t buflen = LINE_MAX;
    char* buf = calloc(buflen, sizeof(char));
    buf = fgets(buf, buflen, stdin);
    if (buf == NULL) {
        free(buf);
        return hdlReadInErr(0);
    }   

    //successfully parsed argument number
    int an = sscanf(buf, "%s %s %ld %ld %s", cmd, ltype, start, length, whence);
    int isWhenceGiven = 0;

    switch (an) {

        // If sscanf says that there are 4 arguments (whence was NOT given)
        case 4:
            *whence = 's'; // SEEK_SET is the default value for whence
            break;

        // If sscanf says that there are 5 arguments (whence was given)
        case 5:
            isWhenceGiven = 1;
            break;

        case 1:
            //* If user did not enter '?' or 'Q (quit_char)' then having only 1 parameter is not valid.
            //* The program will then not enter the if clause, i.e. => not break => continue to default branch => finally return -1 with errno set to EINVAL.
            if (*cmd == HELP_CHAR) {
                //if user seeked for help (entered ?) => returns 2 to indicate that. else does what was described above.
                free(buf);
                return 2;

            } else if (*cmd == QUIT_CHAR) {
                printf("\n%c was pressed, exiting...\n", QUIT_CHAR);
                exit(EXIT_SUCCESS);
            }
            // else: just go into default branch because there is no break/return ...
        default:
            errno = EINVAL;
            fprintf(stderr, "%s: Expecting between 4 and 5 inputs, received:%d\n", strerror(EINVAL), an);
            free(buf);
            return -1;
            break;
    }
   
    int isArgValid = isLockInputValid(cmd, ltype, *start, *length, isWhenceGiven ? whence : NULL);

    if (isArgValid <= 0) {
        free(buf);
        return -1;
    }

    free(buf);
    return EXIT_SUCCESS;
}



// ------- PARSING ARG DIRECTLY PASSED TO PROGRAM

const char* parseArgv(int argc, char* argv[]) {
    const char* parsedArg;
    if (checkArgsNb(argc) != 0) {
        fprintf(stderr, "%s: Not Enough arguments: Expecting at least %d.\n", argv[0], MIN_ARG_NB);
        exit(EXIT_FAILURE);
    }

    // relevant arg i.e. not program name. (Waiting only 1 arg)
    char* arg = argv[1];

    int argLen = strlen(arg);
    if (argLen <= 0 || argLen >= PATH_MAX) {
        errno = EINVAL;
        exit(errno);
    }
    // argv[i] is not destination i.e. last element and argv[i] does not exists
    if (!exists(arg)) {
        errno = ENOENT;
        fprintf(stderr, "\"%s\" Invalid argument: %s", arg, strerror(ENOENT));
        return NULL;
    }
    // returns absolute path if it was computed correctly.
    parsedArg = realpath(arg, NULL);
    if (parsedArg == NULL) {
        int savedErr = errno;
        fprintf(stderr, "Cannot resolve file %s: %s\n", arg, strerror(savedErr));
        exit(errno);
    }
    FILE_SIZE = getFileSize(parsedArg);
    
    if (FILE_SIZE < 0) {
        fprintf(stderr, "Cannot get size of given file %s, ==> Max size will then be infered.\n", parsedArg);
        FILE_SIZE = SSIZE_MAX;
    }
    return parsedArg;
}

