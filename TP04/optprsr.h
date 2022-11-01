
/**
 * @brief Check if there is at least 1 argument.
 * @param argc same as argc in main
 * @param fileName argv[0]
 * @return 0 if success otherwise code 22 (Invalid argument)
 */
int checkEnoughArgs(int argc, char* file_name);

/**
 * Parses & extract files/folder given at main function and return a copy of them.
 * (do not keep optional arguments)
 * @param argc the number of arguments passed to the program
 * @param argv the array of arguments passed to the program
 * @param fileNb variable into which store the amount of parsed file to process
 * 
 * @return Copy of parsed arguments.
 */
const char** parseArgs(int argc, char* argv[], int* fileNb);

/**
 * Parses the optional arguments (i.e. -f, -a) and returns 'state', an int describing what was given.
 * @param argc number of argument in main
 * @param argv arguments given when launching program
 * @return int : 0 if no optional argument were passed, 
 *               1 if -f was passed, 2 if -a, were passed, 3 if both were passed. -1 if error
 */
int parseOptArgs(int argc, char* argv[]);