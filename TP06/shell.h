
/**
 * Changes the current working directory to the one specified by the path argument
 * 
 * @param path The path to the directory to change to.
 * 
 * @return Exit code. 0 for success, -1 for error.
 */
int cd(const char* path);


/**
 * Returns the current working directory
 * 
 * @return The current working directory.
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
