
/**
 * Changes the current working directory to the one specified by the path argument
 * 
 * @param path The path to the directory to change to.
 * 
 * @return Exit code. 0 for success, -1 for error.
 */
int cd(const char* path);


/**
 * Wrapper for the 'getcwd()' system call.
 * 
 * @param buff - buffer in which to store current path
 * 
 * @return Current working directory. (output is the same as typing 'pwd' in any shell). Or NULL on error.
 */
const char* pwd();


/**
 * Print the current working directory to console output
 */
void prettyPrintPath();

/** init shell, todo: remove this after when proper struct was implemented */
void sh_init();

//
// NB: returned string as allocated with malloc. Don't forget to free.