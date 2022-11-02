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
 * Reads a line from stdin, and parses it into the given variables, then checks if the parsed values
 * are valid
 * 
 * @param INP_FORMAT The format string for the input.
 * @param cmd command to be executed (g, s, w)
 * @param ltype type of lock to be placed. (r, w, u)
 * @param start start of lock location 
 * @param stop end of lock location
 * @param whence relatively to what are start and end computed ? (s, c, e)
 * 
 * @return 0 if success, -1 if error (errno is set accordingly). Or 2 if user entered an '?' to get help.
 */
int parseInput(const char* INP_FORMAT, char* cmd, char* ltype, int* start, int* stop, char* whence);

