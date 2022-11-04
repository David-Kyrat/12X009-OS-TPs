#include <stdio.h>
#include <stdlib.h>

// Managing file locks
#include <sys/file.h>
#include <unistd.h>
#include <fcntl.h>


// From TP4 (4.1. Utilisation)
struct flock {
    short l_type; /* Type of lock: F_RDLCK,F_WRLCK, F_UNLCK */
    short l_whence; /* How to interpret l_start: SEEK_SET, SEEK_CUR, SEEK_END */
    off_t l_start; /* Starting offset for lock */
    off_t l_len; /* Number of bytes to lock */
    pid_t l_pid; /* PID of process blocking our lock (set by F_GETLK and F_OFD_GETLK)
*/
};


// TODO: finish lock function

// example from lecture 7: I/O
int LOCK(int fd, int file) {
    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = file;
    fl.l_len = 1;
    return fcntl(fd, F_SETLKW, &fl);
}