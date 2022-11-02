
/**
 * @brief Check if there is at least 1 argument.
 * @param argc same as argc in main
 * @return 0 if success otherwise code 22 (Invalid argument)
 */
//int checkArgsNb(int argc, char* file_name);

/**
 * Parses & extract path of file given at main function and return a copy of it.
 * (does a malloc remember to free.)
 * @param argc the number of arguments passed to the program
 * @param argv the array of arguments passed to the program
 * 
 * @return Copy of parsed arguments.
 */
const char* parseArgs(int argc, char* argv[]);

/**
 * Parses the optional arguments (i.e. -f, -a) and returns 'state', an int describing what was given.
 * @param argc number of argument in main
 * @param argv arguments given when launching program
 * @return int : 0 if no optional argument were passed, 
 *               1 if -f was passed, 2 if -a, were passed, 3 if both were passed. -1 if error
 */
//int parseOptArgs(int argc, char* argv[]);