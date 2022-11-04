#include <stdio.h>
#include <stdlib.h>

// Managing file locks
#include <sys/file.h>
#include <unistd.h>
#include <fcntl.h>

#include "inp.h"
#include "lock.h"

struct flock {
    short l_type; /* Type of lock: F_RDLCK,F_WRLCK, F_UNLCK */
    short l_whence; /* How to interpret l_start: SEEK_SET, SEEK_CUR, SEEK_END */
    off_t l_start; /* Starting offset for lock */
    off_t l_len; /* Number of bytes to lock */
    pid_t l_pid; /* PID of process blocking our lock (set by F_GETLK and F_OFD_GETLK) */
};

// example from lecture 7: I/O

int lock(int fd, Inp* input) {
    
    // props[0] to props[2] of Inp contains each 'command' 'lock type' 'whence'
    flock fl;

    fl.l_type = inp_ltp(input);
    fl.l_whence = inp_whc(input);
    fl.l_start = inp_start(input);
    fl.l_len = inp_stop(input) - inp_start(input);
    fl.l_pid = getpid();

    return fcntl(fd, inp_cmd(input), &fl);
}