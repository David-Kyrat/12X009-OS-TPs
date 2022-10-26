/**
 * @file util.h
 * @brief Utility functions mostly used for error handling
 * 
 */

/**
 * Checks if alloc (malloc, calloc ...) returned Null, if it did prints error message error message to stderr and exit with error ENOMEM
 * @param allocReturn The return value of alloc.
 * @return allocReturn if it's not null, exit otherwise
 */
void* tryalc(void* allocReturn);

/**
 * Utility function for basic handling of error related to file opening.
 * 
 * Prints an error message to stderr and exits with the error code if needsExit is true, otherwise
 * just returns -1
 * @param filename Name of the file that could not be opened.
 * @param needsExit int, If non-zero, program will exit with the saved error code from errno. (otherwise do nothing)
 * @return -1 or, exits with saved errno, if needsExit was != 0.
 */
int hdlOpenErr(const char* filename, int exit);

/**
 * Utility function for basic handling of error related to file closing.
 * 
 * Prints an error message to stderr and exits with the error code if needsExit is true, otherwise
 * ust returns -1
 * @param filename Name of the file that could not be closed.
 * @param needsExit int, If non-zero, program will exit with the saved error code from errno. (otherwise do nothing)
 * @return -1 or, exits with saved errno, if needsExit was != 0.
 */
int hdlCloseErr(const char* filename, int exit);

/**
 * Utility function for basic handling of error related to file reading.
 * Prints an error message to stderr, closes the file descriptor if needed, and exits if needed
 * 
 * @param filename Name of the file that was being read from
 * @param needsExit If non-zero, the program will exit with the error code of the error that
 * occurred.
 * @param needsClose if non-zero, function will attempt to close the given fd (also handles closing error if necessary)
 * @param fd int, file descriptor of file being read from (can be null/0 if needsClose is 0)
 * @return -1 or, exits with saved errno, if needsExit was != 0.
 */
int hdlReadErr(const char* filename, int needsExit, int needsClose, int fd);

/**
 * Utility function for basic handling of error related to file writing.
 * Prints an error message to stderr, closes the file descriptor if needed, and exits if needed
 * 
 * @param filename Name of the file that was being read from
 * @param needsExit If non-zero, the program will exit with the error code of the error that
 * occurred.
 * @param needsClose if non-zero, function will attempt to close the given fd (also handles closing error if necessary)
 * @param fd int, file descriptor of file being written to (can be null/0 if needsClose is 0)
 * @return -1 or, exits with saved errno, if needsExit was != 0.
 */
int hdlWriteErr(const char* filename, int needsExit, int needsClose, int fd);

/**
 * Utility function for basic handling of error related to file copying.
 * 
 * Prints an error message to stderr, close both given file descriptors if needsClose is non-zero 
 * and exits with the error code if needsExit is true, otherwise just returns -1.
 * Note: Also handles closing error if necessary
 * 
 * @param from Name of the file being copied
 * @param to Name of the file being copied to
 * @param needsExit int, If non-zero, program will exit with the saved error code from errno. (otherwise do nothing)
 * @param needsClose int, If non-zero, function will close both file descriptors given as argument
 * @param fd_from int, file descriptor of from (can be null/0 if needsClose is 0)
 * @param fd_from int, file descriptor of to (can be null/0 if needsClose is 0)
 * 
 * @return -1 or, exits with saved errno, if needsExit was != 0.
 */
int hdlCopyErr(const char* from, const char* to, int needsExit, int needsClose, int fd_from, int fd_to);

/**
 * Prints error message to stderr and return -1
 * @param current path of file to be concatenated with some other string (usually parent directory's path)
 * @return -1
 */
int hdlCatErr(const char* current);
