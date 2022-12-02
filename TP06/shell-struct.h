#ifndef __SHELL__
#define __SHELL__

#include <limits.h>
#include <sys/types.h>


/**
 * Represents a Shell with a crt_path (pwd) a foreground job and 
 * a background_job. Crt_path default value is where the executable was called
 * and foregroud_job & background_job default value are -1.
 * 
 */
typedef struct Shell Shell;

struct Shell {
    /** Contains the cwd (of Size PATH_MAX) */
    char* crt_path;
    pid_t foreground_job;
    pid_t background_job;

};

/**
 * Shell primary constructor. Return a pointer to a new instance
 * of Shell with crt_path initialized at where the executable was called
 * and no foregroud_job nor background_job (-1 value)
 * 
 * @return pointer to a new instance of Shel
 */
Shell* new_Shell();

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
