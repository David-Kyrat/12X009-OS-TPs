
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
char* parseArgsAsString(int argc, char* argv[]);


/**
 * Extracts the file names from the command line arguments and returns them as an array of strings
 * 
 * @param argc Number of arguments passed to the program.
 * @param argv Array of arguments given to the program.
 * @param opt Option that was given as argument
 * @param fileAmnt (unsigned int) Variable into which store the amount of given files.
 *                  [ NB: if fileAmnt stays 0 at the end of the call, then 'f' option was not provided ]
 * 
 * @return array of file paths
 */
int parseOptArgs(int argc, char* argv[], int* fileAmnt);


/**
 * Call parseOptArgs to parse given options then if "-f" was not provided, 
 * call parseArgsAsString to interpret all given argument (that are not options) as 1 single string
 * i.e. "-f file1 file2 [-t <hashMethod>]" parses file1, file2 and hashMethod as separate things to hash.
 * and "s1 s2 [-t <hashMethod>]" parses "s1 s2" as 1 string to hash.
 *
 * 
 * @param argc Number of arguments passed to the program.
 * @param argv Array of arguments given to the program.
 * @param givenFilesToHash pointer to string array into which store the parsed files to hash or null if -f was not provided. Does a malloc. Remember to free! (when according option was chosen)
 * @param fileAmnt Variable into which store the amount of given files.
 * @param givenStringToHash pointer to string into which store the parsed string to hash or null if -f was provided. Does a malloc. Remember to free! (when according option was chosen)
 * @return 0 if success else error code
 */
int parseArgs(int argc, char* argv[], char** givenFileToHash[], int* fileAmnt, char* givenStringToHash[]);

/**
 * Return hashMethod parsed by the optionParser or the defaultHashMethod if none were given as argument (i.e. hahsMethod == NULL)
 * @param defaultHashMethod 
 */
char* getHashMethod(char* defaultHashMethod);