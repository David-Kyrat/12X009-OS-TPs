#ifndef __SHELLS__
#define __SHELLS__

#include <limits.h>
#include <sys/types.h>


/**
 * Represents a Shell with a crt_path (pwd) a foreground job and 
 * a background_job. Crt_path default value is where the executable was called
 * and foreground_job & background_job default value are -1.
 * 
 */
typedef struct Shell Shell;



/**
 * Shell primary constructor. Return a pointer to a new instance
 * of Shell with crt_path initialized at where the executable was called
 * and no foreground_job nor background_job (-1 value)
 * 
 * @return pointer to a new instance of Shel
 */
Shell* new_Shell();

/**
 * Returns the current working directory
 * 
 * @return The current working directory.
 */
const char* pwd(Shell* sh);


/**
 * Call 'readParseIn()' to get the parsed user input => 
 * then resolve the requested command / program to launch & execute them accordingly.
 * 
 * @return Exit code. 0 on success, -1 otherwise.
 */
int getAndResolveCmd();


// =================== GETTERS & Other =====================

/**
 * @param sh pointer to Shell instance
 * @return background_job field for 'sh'
 */
pid_t get_BJ(Shell* sh);

/**
 * @param sh pointer to Shell instance
 * @return foreground_job field for 'sh'
 */
pid_t get_FJ(Shell* sh);

/**
 * Print the current working directory to console output
 */
void prettyPrintPath();

//
// NB: returned string as allocated with malloc. Don't forget to free.



#endif /* __SHELLS__ */
