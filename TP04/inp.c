#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "inp.h"
#include "optprsr.h"
#include "util.h"


const char* INP_FORMAT = "%c %c %d %d %c";
int strArgNb = 3;  //* nb of char argument stored in field 'props'

struct Inp {
    /**
     * Contains concatenation of mode etc...
     * props[0] to properties[2] contains each 'command' 'lock type' 'whence'.
     * Inp struct is opaque to enable "setting 'props' private".
     */
    char* props;

    int start, stop;
};

char inp_cmd(Inp* input) { return input->props[0]; }
char inp_ltp(Inp* input) { return input->props[1]; }
char inp_whc(Inp* input) { return input->props[2]; }
int inp_start(Inp* input) { return input->start; }
int inp_stop(Inp* input) { return input->stop; }

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

    printf("enter cmd ltype start stop whence\n");
    printf("PID=256> ");
    errno = 0;
    if (parseInput(INP_FORMAT, &inp->props[0], &inp->props[1], &inp->start, &inp->stop, &inp->props[2]) < 0) {
        int savedErr = errno;
        fprintf(stderr, "Error when parsing user input : %s\n", strerror(savedErr));
        return NULL;
    }
    /* char* buf = calloc(MAX_INPUT, sizeof(char));
    fgets(buf, MAX_INPUT, stdin);
    int an = sscanf(buf, INP_FORMAT, &inp->props[0], &inp->props[1], &inp->start, &inp->stop, &inp->props[2]);

    switch (an) {
        case 4:
            inp_print(inp);
            printf("whence not given\n");
            break;
        case 5:
            inp_print(inp);
            printf("whence given\n");
            break;

        default:
            errno = EINVAL;
            fprintf(stderr, "%s: Expecting between 4 and 5 inputs\n", strerror(EINVAL));
            inp_free(inp);
            free(buf);
            return NULL;
            break;
    } */


    return inp;
}


void inp_free(Inp* inp) {
    free(inp->props);
    free(inp);
}
