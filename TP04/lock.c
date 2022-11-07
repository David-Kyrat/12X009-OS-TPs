#include <stdio.h>
#include <stdlib.h>

// Managing file locks
#include <fcntl.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include "inp.h"
#include "lock.h"

// example from lecture 7: I/O

int hdl_lk_conflict(int savedErr, struct flock fl, Inp* inp) {
    if (savedErr == EACCES || savedErr == EAGAIN) {
        const char type = fl.l_type == 0 ? 'r' : 'w';
        fprintf(stderr, "Denied by %c lock on %ld:%ld (held by PID %d)\n", type, fl.l_start, fl.l_len, fl.l_pid);
    }
    return -1;
}

int lock(int fd, Inp* input) {
    struct flock fl;

    int fl_cmd = toFlock_cmd(input);
    fl.l_type = toFlock_ltype(input);
    fl.l_whence = toFlock_whence(input);
    fl.l_start = inp_start(input);
    fl.l_len = inp_length(input);
    fl.l_pid = getpid();
    
    
    if (fl_cmd == 'w') {
        printPid(0);
        printf("Waiting for lock to be released...\n");
    }
    
    int res = fcntl(fd, fl_cmd, &fl);
    printPid(0);
    switch (fl_cmd) {
        case F_GETLK:
            if (res < 0){
                int savedErr = errno;
                fprintf(stderr, "%s: Could not get lock at %ld:%ld (whence = %c)\n", strerror(savedErr), inp_start(input), inp_length(input), inp_whc(input));
                return hdl_lk_conflict(savedErr, fl, input);
            }
            else printf("%c lock present\n", inp_ltp(input));
            break;

        case F_SETLK:
            if (res < 0) {
                int savedErr = errno;
                if (inp_ltp(input) == 'u')
                    fprintf(stderr, "%s: Could not unlock at %ld:%ld (whence = %c)\n", strerror(savedErr), inp_start(input), inp_length(input), inp_whc(input));
                else
                    fprintf(stderr, "%s: Could not add %c lock at %ld:%ld (whence = %c)\n", strerror(savedErr), inp_ltp(input), inp_start(input), inp_length(input), inp_whc(input));

                return hdl_lk_conflict(savedErr, fl, input);
            } else 
                printf("Successfully set lock\n");
            break;    

        case F_SETLKW:
            if (res < 0) {
                int savedErr = errno;
                if (inp_ltp(input) == 'u')
                    fprintf(stderr, "%s: Could not wait & unlock at %ld:%ld (whence = %c)\n", strerror(savedErr), inp_start(input), inp_length(input), inp_whc(input));
                else
                    fprintf(stderr, "%s: Could not wait & add %c lock at %ld:%ld (whence = %c)\n", strerror(savedErr), inp_ltp(input), inp_start(input), inp_length(input), inp_whc(input));

                return hdl_lk_conflict(savedErr, fl, input);
            } else 
                printf("Successfully set lock\n");
            break;    

        default:
            return -1;
    }
/*     if (fl.l_type == F_GETLK) {
        if (res == -1) printf("No lock found\n");
        else printf("Got lock\n");
    }

    if (fl.l_type == F_SETLK) {
        if (res == -1) printf("Unable to set lock\n");
        else printf("Successfully locked/unlocked by %d\n", fl.l_pid);
    }
    
    if (fl.l_type == F_SETLKW) {
        if (res == -1) printf("Waiting for a lock to be released\n");
        else printf("Successfully locked/unlocked by %d\n", fl.l_pid);
    } */

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
