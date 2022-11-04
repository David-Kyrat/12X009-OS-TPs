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

const char* INP_FORMAT = "%c %c %ld %d %c";
//static const char HELP_CHAR = '?';
//* default message displayed when beginning program
const char DEF_MSG[] = "Enter ? for help or Q to exit.";

 char* HELP_MSG /* ="Format: cmd l_type start length [whence(optional)] \
'cmd' --- 'g' (F_GETLK), 's' (F_SETLK), or 'w' (F_SETLKW) \
'l_type' --- 'r' (F_RDLCK), 'w' (F_WRLCK), or 'u' (F_UNLCK) \
'start' --- lock starting offset \
'length' --- number of bytes to lock \
'whence' --- 's' (SEEK_SET, default), 'c' (SEEK_CUR), or 'e' (SEEK_END)"*/;

char* HELP_MSGS[6];

const int strArgNb = 3;  //* nb of char argument stored in field 'props'

int isHelpMessInit = 0;

void initHelpMess() {

    for(int i = 0; i < sizeof HELP_MSGS; i++) HELP_MSGS[i] = calloc(512, sizeof(char));

    HELP_MSGS[0] = strdup("\n\t Format: cmd l_type start length [whence]");
    HELP_MSGS[1] = strdup("\n\t 'cmd' --- 'g' (F_GETLK), 's' (F_SETLK), or 'w' (F_SETLKW)");
    HELP_MSGS[2] = strdup("\n\t 'start' --- lock starting offset");
    HELP_MSGS[3] = strdup("\n\t 'length' --- number of bytes to lock");
    HELP_MSGS[4] = strdup("\n\t 'whence' --- 's' (SEEK_SET, default), 'c' (SEEK_CUR), or 'e' (SEEK_END)\n");

    HELP_MSG = calloc(sizeof HELP_MSGS * 512 + 1, sizeof(char));


    for (int i = 0; i < 5; i++) {
        strncat(HELP_MSG, HELP_MSGS[i], 512);
    }

}



struct Inp {
    /**
     * Contains concatenation of mode etc...
     * props[0] to props[2] contains each 'command' 'lock type' 'whence'. 
     * Inp struct is opaque to enable "setting 'props' private".
     */
    char* props;
    long start, stop;
};

/**
 * Prints process id in specific manner
 * @param in, if ('in') then prints formatted as waiting output (i.e. PID=%d> ), else as printing ouput (i.e. [PID=%d] )
 */
void printPid(int in) {
    char* msg = in ? "PID=%d> " : "[PID=%d] ";
    printf(msg, getpid());
}

int isWhenceGiven(char* props) {
    return props[2] && props[2] != '\0';
}

void inp_print(Inp* inp) {
    printf("[ cm:%c, tp:%c, st:%ld, sp:%ld", inp->props[0], inp->props[1], inp->start, inp->stop);
    if (isWhenceGiven(inp->props)) printf(", wh:%c", inp->props[2]);
    printf(" ]\n");
}

Inp* inp_askUser() {
    Inp* inp = malloc(sizeof(Inp));
    inp->props = calloc(strArgNb + 1, sizeof(char));

    printf("%s\n", DEF_MSG);
    printPid(1);
    errno = 0;
    int exitCode = parseInput(INP_FORMAT, &inp->props[0], &inp->props[1], &inp->start, &inp->stop, &inp->props[2]);
    if (exitCode < 0) {
        int savedErr = errno;
        fprintf(stderr, "Error when parsing user input : %s\n", strerror(savedErr));
        return NULL;
    }
    if (exitCode == 2) {
        //user entered '?' 
        if (! isHelpMessInit){ initHelpMess(); isHelpMessInit = 1;}
        
        printf("%s\n", HELP_MSG);

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


// ---- Initializing long string like the help message to display on '?' is very cumbersome and can produce various 
// ---- really dumb errors, so it is hard coded here in an not very elegant way.
