#include <errno.h>
#include <getopt.h>
#include <limits.h>  //PATH_MAX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "functions.h"
#include "files.h"
#include "optprsr.h"
#include "util.h"


//static const char HELP_CHAR = '?', QUIT_CHAR = 'Q';
//const char* OPT_STRING = "fa";
const int MIN_ARG_NB_SERV = 1, MAX_ARG_NB_SERV = 1;
const int MIN_ARG_NB_CLIENT = 2, MAX_ARG_NB_CLIENT = 2;
const char* USAGE_MSG_SERV = "Usage: %s <portNumber> (2Bytes integer in [1024, 65535]) \n";
const char* USAGE_MSG_CLIENT = "Usage: %s <ip-address>(in decimal)  <portNumber> (2Bytes integer in [1024, 65535]) \n";

const int PORT_SIZE = 2;

//* Size of the file given when calling program
size_t FILE_SIZE; 

/**
 * @brief Check if there is at least 'minArgNb' and at max 'maxArgNb' argument.
 * @param argc same as argc in main
 * @return 0 if success otherwise code 22 (Invalid argument)
 */
int checkArgsNb(int argc, const int minArgNb, const int maxArgNb) {
    int relevantNb = argc - 1;
    if (relevantNb < minArgNb || relevantNb > maxArgNb) {
        errno = EINVAL;
        return EINVAL;
    }
    return 0;
}

int checkArgsNbServ(int argc) {
    return checkArgsNb(argc, MIN_ARG_NB_SERV, MAX_ARG_NB_SERV);
}
int checkArgsNbClient(int argc) {
    return checkArgsNb(argc, MIN_ARG_NB_CLIENT, MAX_ARG_NB_CLIENT);
}


int parseArgv(int argc, char* argv[], int isServ) {
    int checkArgsErr = isServ ? checkArgsNbServ(argc) : checkArgsNbClient(argc);
    if (checkArgsErr < 0) setRErrno(EINVAL);

    int port_idx = isServ ? 1 : 2; // position of port value (as a string) in argv[]
    char* port_str = strndup(argv[port_idx], strlen(argv[port_idx]));
    int port = atoi(port_str);
    if (!isPortValid(port)) return -1;

    // parsing ip address if client called (i.e. isServ == 0)
    if (!isServ) {
        
    }

}

/*int parseInput(const char* INP_FORMAT, char* cmd, char* ltype, long* start, long* length, char* whence) {
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
    

   //TODO: IMPLEMENT
    int isArgValid = 0; //isLockInputValid(cmd, ltype, *start, *length, isWhenceGiven ? whence : NULL);

    if (isArgValid <= 0) {
        free(buf);
        return -1;
    }

    free(buf);
    return EXIT_SUCCESS;
}*/



// ------- PARSING ARG DIRECTLY PASSED TO PROGRAM

/* const char* parseArgv(int argc, char* argv[]) {
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
    // returns absolute path if it was computed correctly.
    parsedArg = realpath(arg, NULL);
    if (parsedArg == NULL) {
        int savedErr = errno;
        fprintf(stderr, "Cannot resolve file %s: %s\n", arg, strerror(savedErr));
        return NULL;
    }
    FILE_SIZE = getFileSize(parsedArg);
    
    if (FILE_SIZE < 0) {
        fprintf(stderr, "Cannot get size of given file %s, ==> Max size will then be infered.\n", parsedArg);
        FILE_SIZE = SSIZE_MAX;
    }
    return parsedArg;
}
 */
