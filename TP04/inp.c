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

const char* INP_FORMAT = "%c %c %d %d %c";
//static const char HELP_CHAR = '?';
//* default message displayed when beginning program
const char DEF_MSG[] = "Enter ? for help or Q to exit.";

const int strArgNb = 3;  //* nb of char argument stored in field 'props'

struct Inp {
    /**
     * Contains concatenation of mode etc...
     * props[0] to properties[2] contains each 'command' 'lock type' 'whence'. 
     * Inp struct is opaque to enable "setting 'props' private".
     */
    char* props;
    int start, stop;
};

void printPid() { printf("PID=%d> ", getpid()); }

int isWhenceGiven(char* props) {
    return props[2] && props[2] != '\0';
}

void inp_print(Inp* inp) {
    printf("[ cm:%c, tp:%c, st:%d, sp:%d", inp->props[0], inp->props[1], inp->start, inp->stop);
    if (isWhenceGiven(inp->props)) printf(", wh:%c", inp->props[2]);
    printf(" ]\n");
}

Inp* inp_askUser() {
    Inp* inp = malloc(sizeof(Inp));
    inp->props = calloc(strArgNb + 1, sizeof(char));

    printf("%s\n", DEF_MSG);
    printPid();
    errno = 0;
    int exitCode = parseInput(INP_FORMAT, &inp->props[0], &inp->props[1], &inp->start, &inp->stop, &inp->props[2]);
    if (exitCode < 0) {
        int savedErr = errno;
        fprintf(stderr, "Error when parsing user input : %s\n", strerror(savedErr));
        return NULL;
    }
    if (exitCode == 2) {
        //user entered '?' 
        //TODO: display help message
        printf(" -- temporary help message --\n");
        //if usered entered only '?' the attributes wont be correctly initialized, hence why we're doing it here.
        for (int i = 1; i < strArgNb; i++) inp->props[i] = '\0'; //terminating 'props' string at first char which should be '?'
        inp->start = -1; 
        inp->stop = -1;
    }

    return inp;
}

// ------------ GETTERS --------------------

char inp_cmd(Inp* input) { return input->props[0]; }
char inp_ltp(Inp* input) { return input->props[1]; }
char inp_whc(Inp* input) { return input->props[2]; }
int inp_start(Inp* input) { return input->start; }
int inp_stop(Inp* input) { return input->stop; }

// ------------ END GETTERS --------------------

void inp_free(Inp* inp) {
    free(inp->props);
    free(inp);
}
