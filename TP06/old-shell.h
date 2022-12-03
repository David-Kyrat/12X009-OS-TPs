
//TODO remove cd from shell.h
//! exported here only for testing purposes.

int cd(const char* path);


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
int sh_getAndResolveCmd();


/**
 * Print the current working directory to console output
 */
void sh_prettyPrintPath();

/** init shell, todo: remove this after when proper struct was implemented */
void sh_init();

//
// NB: returned string as allocated with malloc. Don't forget to free.
