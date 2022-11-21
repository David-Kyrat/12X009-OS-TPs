#include "inp.h"


/**
 * Simplify use of fcntl to handle locking related operations.  
 * 
 * Take a file descriptor and a user input then call 'fcntl' to perform what was asked by user (i.e. get/set lock etc...)
 * on the file pointed to by the given file descriptor
 * 
 * @param fd file descriptor
 * @param input Inp struct, Contains the attribute of the flock struct and cmd to execute entered by user
 * 
 * @return 0 if success, -1 if error
 */
int lock(int fd, Inp* input);