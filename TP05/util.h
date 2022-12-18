#ifndef __UTIL__
#define __UTIL__

#include <errno.h>

/**
 * @file util.h
 * @brief Utility functions mostly used for error handling
 * 
 */
// 0:default,  1:red, 2:Green,  3:Blue, 4:Purple, 5:yellow,  6:cyan,  7:grey
static const char* colors[] = {"\033[0m", "\033[0;31m", "\033[0;32m", "\033[0;34m", "\033[0;35m", "\033[0;33m", "\033[0;36m", "\033[0;30m"};
/** Macro that Prints given message to stderr. Message should be printf formatted and the first arg is always strerror(savedErr) where savedErr is the saved value of errno.
* Usage: 'printErr("%s, %d: port number not valid\n", port)' first %s will be 'strerror(savedErr)' */
#define printErr(mess, args...) \
    {int savedErr = errno;  \
    fprintf(stderr, "%s\t", colors[1]);\
    fprintf(stderr, mess, strerror(savedErr == 0 ? EXIT_FAILURE : savedErr), args);\
    fprintf(stderr, "%s", colors[0]);}
    // if savedErr == 0 then strerror(savedError) will return "success" which wouldn't make any sense for an error message.

//* Macro that Prints given (printf formatted) message to stderr and returns -1. See 'printErr' for more info
#define printRErr(mess, args...) \
    { printErr(mess, args); return -1; }


#define MIN(a, b) (a < b ? a : b)


/** 
 * Macro that stores errno into the variable 'savedErr' then checks whether the given error 'condition' is true, 
 * then prints the error message and given args to stderr and returns -1 (error) or 0 (sucess) accordingly.
 * 
 #define check_hdlError(condition, args...)  \
    int savedErr = errno;                   \
    if (condition) printRErr(stderr, args); \
    else return 0; */

/**
 * Utility function called to check if an *alloc() call returned null. => print "Cannot allocated Memory" to stderr and exit with exit code ENOMEM. 
 * return ("nothing" i.e. make program exit with code ENOMEM)
 */
void hdlOOM();

/**
 * If *alloc (malloc, calloc ...) returned null exits (i.e. hdlOOM()) if it didn't return the alloc return.
 * mostly useful to be able do things like ' smth = tryalc( *alloc(...) ) ' (i.e. directly doing and the malloc and the test)
 * 
 * @param allocReturn The return value of alloc.
 * @return allocReturn if it's not null, exit otherwise
 */
void* tryalc(void* allocReturn);


/**
 * Perform a memset 0 on 'n' bytes of 'data'
 */
void memzero(void * data, unsigned long n);


/**
 * Utility function for basic error handling
 * 
 * Justs Prints an error message to stderr return -1
 * @return -1
 */
int hdlBasicErr();

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
 * Utility function for basic handling of error related to reading from stdin.
 * Reading from standard input failed => print an error message to standard error and return -1
 * @param needsExit If non-zero, the program will exit with the error code of the error that
 * occurred.
 * @return -1 or, exits with saved errno, if needsExit was != 0.
 */
int hdlReadInErr(int needsExit);

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

/**
 * shortcut for lstat, also handles error
 * @return stat structure of path's inode. If an error happened => the 'st_size' attribute is set to -1
 */
struct stat lstat_s(const char* path);

/**
 * shortcut for stat, also handles error
 * @return stat structure of path's inode. If an error happened => the 'st_size' attribute is set to -1
 */
struct stat stat_s(const char* path);

/**
 * Comparator of date. i.e. compare time 'ts2' and 'ts1' and return an integer (1, 0, -1)
 * describing if ts2 > ts1, ts2 == ts1 or if ts2 < ts1.
 
 * @param ts2 Second timestamp to compare
 * @param ts1 First timestamp to compare
 * 
 * @return 1, 0 or -1 if ts2 is respectively Greater, Equal to or Less than ts1.
 */
int dateCmpr(struct timespec ts2, struct timespec ts1);

/**
 * Manuel advise of using strtol instead of atoi when converting (strtol can differentiate 0 value and an error).
 * => Converts to int and resolve errors if necessary.
 * @param str string to convert
 * @param base base in which to convert
 * @param result pointer to int in which to store the result
 * @return 0 on success, -1 on error.
 */
int strToInt(const char* str, int base, int* result);

/**
 * Extract substring of 'src' from 0 to substring_length . Memory for substring was allocated with strdup.
 * 
 * @param src The string to extract the substring from.
 * @param stop_idx End of substring. i.e. idx of char after last one
 * 
 * @return src[0 : stop_idx] i.e. substring of length 'stop_idx'
 */
char* strsub(char* src, int stop_idx);


/**
 * Check if 2 strings are equal 
 * @param s1 First string to compare
 * @param s2 Second to string to compare it with
 *
 * @return 1 if they do 0 otherwise
 */
int streq(const char* s1, const char* s2);


void error(const char* msg);

#endif /*__UTIL__*/

