
/** @return const char*, the different options that can be used */
const char* getOptString();

/**
 * Takes an array of strings, a separator string, and returns a string that is the concatenation of
 * all the strings in the array, separated by the separator
 * 
 * @param arr the array of strings to concatenate
 * @param arrSize the size of the array
 * @param sep the separator to use between each element of the array.
 * 
 * @return Concatenation of array
 */
char* catArr(char** arr, int arrSize, char* sep);

/**
 * If malloc returns null, print an error message and exit
 * 
 * @param allocReturn The return value of malloc.
 * @param line The line number of the file where the error occurred.
 * 
 * @return allocReturn if it's not null, exit otherwise
 */
void* tryalc(void* allocReturn, int line);

/**
 * Check if there is at least 1 argument, otherwise exits with code 22 (Invalid argument)
 * @param argc same as argc in main
 * @param fileName argv[0]
 */
void checkEnoughArgs(int argc, char* fileName);

/**
 * Handle case where user does not use the -f flag. (i.e. takes every given string and hash them as 1)
 * Takes array of strings from main function and concatenates them into a single string - the string to hash
 * 
 * @param argc the number of arguments passed to the program
 * @param argv the array of arguments passed to the program
 * 
 * @return The String to hash
 */
char* parseSingleArg(int argc, char* argv[]);


/**
 * Extracts the file names from the command line arguments and returns them as an array of strings
 * 
 * @param argc Number of arguments passed to the program.
 * @param argv Array of arguments given to the program.
 * @param opt Option that was given as argument
 * @param fileAmnt Variable into which store the amount of given files
 * 
 * @return array of file paths
 */
char** parseOptArgs(int argc, char* argv[], char opt, int* fileAmnt);
