#include <stdio.h>
#include <stdlib.h>

// Managing file locks
#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>

#include "inp.h"
#include "lock.h"

// example from lecture 7: I/O

int lock(int fd, Inp* input) {
    struct flock fl;

    fl.l_type = toFlock_ltype(input);
    fl.l_whence = toFlock_whence(input);
    fl.l_start = inp_start(input);
    fl.l_len = inp_length(input);
    fl.l_pid = getpid();
    

    int result = fcntl(fd, toFlock_cmd(input), &fl);

    if (fl.ltype == F_GETLK) {
        if (result == -1) printf("No lock found\n");
        else printf("Got lock\n");
    }

    if (fl.ltype == F_SETLK) {
        if (result == -1) printf("Unable to set lock\n");
        else printf("Successfully locked/unlocked by %d\n", fl.l_pid);
    }
    
    if (fl.ltype == F_SETLKW) {
        if (result == -1) printf("Waiting for a lock to be released\n");
        else printf("Successfully locked/unlocked by %d\n", fl.l_pid);
    }

    return 0;


}

// --------- CONVERSION FROM INP TO FLOCK STRUCT --------

/**
 * Converts the cmd character in the input to the corresponding cmd value for the flock
 * function
 * 
 * @param inp The input struct
 * 
 * @return Corresponding cmd value for the flock function
 */
int toFlock_cmd(Inp* inp) {
    switch (inp_cmd(inp)) {
        case 'g':
            return F_GETLK;  // checking if lock present

        case 's':
            return F_SETLK;  // adding lock - non-blocking

        case 'w':
            return F_SETLKW;  //adding lock - blocking

        default:
            //error due to wrong user input is very unlikely to happen given that all parameter of input were already checked before
            //hence no modifying errno here.
            return -1;
    }
}


/**
 * Converts the l_type character in the input to the corresponding l_type value for the flock
 * function
 * 
 * @param inp The input struct
 * 
 * @return Corresponding l_type value for the flock function
 */
int toFlock_ltype(Inp* inp) {
    switch (inp_ltp(inp)) {
        case 'r':
            return F_RDLCK; // read lock
        case 'w':
            return F_WRLCK; // write lock
        case 'u':
            return F_UNLCK; // remove lock

        default:
            return -1;
    }
}

/**
 * Converts the whence character in the input to the corresponding whence value for the flock
 * function
 * 
 * @param inp The input struct
 * 
 * @return Corresponding whence value for the flock function
 */
int toFlock_whence(Inp* inp) {
    switch (inp_whc(inp)) {
        case 's':
            return SEEK_SET;
        case 'c':
            return SEEK_CUR;
        case 'e':
            return SEEK_END;

        default: 
            return -1;
    }
}

/* struct flock inp_to_flock(Inp* inp, int* cmd) {
    char inpCmd = inp_cmd(inp), inpLtype = inp_ltp(inp), inpWhence = inp_whc(inp);
    long start = inp_start(inp), length = inp_length(inp);

    *cmd = toFlock_cmd(inp);
    struct flock fl;
    fl.l_type = toFlock_ltype(input);
    fl.l_whence = toFlock_whence(input);
    fl.l_start = inp_start(input);
    fl.l_len = inp_length(input);
    fl.l_pid = getpid();

}*/
