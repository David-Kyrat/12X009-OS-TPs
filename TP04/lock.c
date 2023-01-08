#include <stdio.h>
#include <stdlib.h>

// Managing file locks
#include <string.h>
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
    if (res >= 0) printPid(0);
    switch (fl_cmd) {
        case F_GETLK:
            if (res < 0){
                int savedErr = errno;
                fprintf(stderr, "[PID=%d] ", getpid());
                fprintf(stderr, "%s: Could not get lock at %ld:%ld (whence = %c)\n", strerror(savedErr), inp_start(input), inp_length(input), inp_whc(input));
                return hdl_lk_conflict(savedErr, fl, input);
            }
            if (fl.l_type == F_UNLCK)
                printf("%c lock not present\n", inp_ltp(input));
            else 
                printf("%c lock present (held by %d)\n", inp_ltp(input), fl.l_pid);
            break;

        case F_SETLK:
            if (res < 0) {
                int savedErr = errno;
                fprintf(stderr, "[PID=%d] ", getpid());
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
                fprintf(stderr, "[PID=%d] ", getpid());
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

    return 0;


}

// --------- CONVERSION FROM INP TO FLOCK STRUCT --------


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
