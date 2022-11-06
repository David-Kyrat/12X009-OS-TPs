#include <stdio.h>
#include <stdlib.h>

// Managing file locks
#include <sys/file.h>
#include <unistd.h>
#include <fcntl.h>

#include "inp.h"
#include "lock.h"

// example from lecture 7: I/O

int lock(int fd, Inp* input) {
    
    // props[0] to props[2] of Inp contains each 'command' 'lock type' 'whence'
    struct flock fl;

    fl.l_type = inp_ltp(input);
    fl.l_whence = inp_whc(input);
    fl.l_start = inp_start(input);
    fl.l_len = inp_stop(input) - inp_start(input);
    fl.l_pid = getpid();

    return fcntl(fd, inp_cmd(input), &fl);

}