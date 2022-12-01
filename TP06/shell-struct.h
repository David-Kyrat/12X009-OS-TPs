#ifndef __SHELL__
#define __SHELL__

#include <limits.h>

typedef struct Shell Shell;

struct Shell {
    char crt_path[PATH_MAX];
    pid_t foreground_job;
    pid_t background_job;

};

/**
 * Returns the current working directory
 * 
 * @return The current working directory.
 */
const char* pwd();


/**
 * Call 'readParseIn()' to get the parsed user input => 
 * then resolve the requested command / program to launch & execute them accordingly.
 * 
 * @return Exit code. 0 on success, -1 otherwise.
 */
int getAndResolveCmd();


/**
 * Print the current working directory to console output
 */
void prettyPrintPath();

/** init shell, todo: remove this after when proper struct was implemented */
void sh_init();

//
// NB: returned string as allocated with malloc. Don't forget to free.



#endif /* __SHELL__ */
