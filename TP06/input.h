void printPid(int in);


/**
 * 
 * Reads user input from stdin and returns it as a string
 * 
 * @return User input or null if nothing could be read
 */
const char* readInput();

 
/**
 * Reads user input from stdin, then parses it (if it was correctly read)
 * and stores the argc into the given 'argc' variable.
 * 
 * @param argc ptr to variable in whcih to store the number of argument
 * @return Array of argument
 */
const char** readParseIn(int* argc);