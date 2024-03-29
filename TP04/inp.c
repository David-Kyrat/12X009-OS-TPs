#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "inp.h"
#include "optprsr.h"
#include "util.h"

const char* INP_FORMAT = "%1s %1s %ld %ld %1s"; //read max 1 char by "string position" (used %s and not %c for mode... to prevent user to enter an int instead of char)
//* default message displayed when beginning program
const char DEF_MSG[] = "Enter ? for help or q to exit.";

const int strArgNb = 3;  //* nb of char argument stored in field 'props'

// Prints the help message in case of '?' entered
const char* HELP_MSG = "\n\t Format:  cmd\tl_type\tstart\tlength\t [whence]" \
"\n\t 'cmd'       ---     'g' (F_GETLK), 's' (F_SETLK), or 'w' (F_SETLKW)" \
"\n\t 'l_type'    ---     'r' (F_RDLCK), 'w' (F_WRLCK), or 'u' (F_UNLCK)" \
"\n\t 'start'     ---     lock starting offset" \
"\n\t 'length'    ---     number of bytes to lock" \
"\n\t 'whence'    ---     's' (SEEK_SET, default), 'c' (SEEK_CUR), or 'e' (SEEK_END)";


struct Inp {
    /**
     * Contains concatenation of mode etc...
     * props[0] to props[2] contains each 'command' 'lock type' 'whence'. 
     * Inp struct is opaque to enable "setting 'props' private".
     */
    char* props;
    long start, length;
    
};


void printPid(int in) {
    char* msg = in ? "PID=%d> " : "[PID=%d] ";
    printf(msg, getpid());
}

int isWhenceGiven(char* props) { return props[2] && props[2] != '\0';}

Inp* inp_askUser() {
    Inp* inp = malloc(sizeof(Inp));
    inp->props = calloc(strArgNb + 1, sizeof(char));

    printf("%s\n", DEF_MSG);
    printPid(1);
    errno = 0;
    int exitCode = parseInput(INP_FORMAT, &inp->props[0], &inp->props[1], &inp->start, &inp->length, &inp->props[2]);
    if (exitCode < 0) {
        int savedErr = errno;
        fprintf(stderr, "Error when parsing user input : %s\n", strerror(savedErr));
        return NULL;
    }
    if (exitCode == 2) {
        //user entered '?' 
        printPid(0);
        printf("%s\n", HELP_MSG);

        //if usered entered only '?' the attributes wont be correctly initialized, hence why we're doing it manually here.
        inp->props[0] = '?';
        for (int i = 1; i < strArgNb; i++) inp->props[i] = '\0'; //terminating 'props' string at first char which should be '?'
        inp->start = -1; 
        inp->length = -1;
    }

    return inp;
}


void inp_print(Inp* inp) {
    printf("[ cm:%c, tp:%c, st:%ld, sp:%ld", inp->props[0], inp->props[1], inp->start, inp->length);
    if (isWhenceGiven(inp->props)) printf(", wh:%c", inp->props[2]);
    printf(" ]\n");
}


// ------------ GETTERS --------------------

char inp_cmd(Inp* input) { return input->props[0]; }
char inp_ltp(Inp* input) { return input->props[1]; }
char inp_whc(Inp* input) { return input->props[2]; }
long inp_start(Inp* input) { return input->start; }
long inp_length(Inp* input) { return input->length; }

// ------------ END GETTERS --------------------


void inp_free(Inp* inp) {
    free(inp->props);
    free(inp);
}
