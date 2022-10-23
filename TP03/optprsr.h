
/**
 * @brief Check if there is at least 1 argument.
 * @param argc same as argc in main
 * @param fileName argv[0]
 * @return 0 if success otherwise code 22 (Invalid argument)
 */
int checkEnoughArgs(int argc, char* file_name);

/**
 * Parses arguments given at main function and return a copy of them.
 * 
 * @param argc the number of arguments passed to the program
 * @param argv the array of arguments passed to the program
 * @param fileNb variable into which store the amount of parsed file to process
 * 
 * @return Copy of parsed arguments.
 */
char** parseArgs(int argc, char* argv[], int* fileNb);