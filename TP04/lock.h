#include "inp.h"

/**
 * Takes a file descriptor and an input struct, and returns the result of calling fcntl with the
 * file descriptor and the input struct's command, lock type, whence, start, and stop
 * 
 * @param fd file descriptor
 * @param input a pointer to a struct Inp
 * 
 * @return The return value is the result of the fcntl() function.
 */

int lock(int fd, Inp* input);