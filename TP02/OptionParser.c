#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "OptionParser.h"

const char* ERR_MESS = "\tUsage: %s [-f file1 file2 ...] [<text to hash>]\n";
const char* OPT_STRING = "f:t:";
const int HASH_METH_MAXLEN = 10;  //* maxium length of string describing hashing method

char* hashMethod = NULL;
int tflag = 0; //* 0 if prog was not called with "-t", 1 otherwise  
// and index of -t argument otherwise

char** filesToHash = NULL;
char* stringToHash = NULL;

/** @return const char*, the different options that can be used */
const char* getOptString() { return OPT_STRING; }

/**
 * Receives the output of the "malloc attempt". If it's null, print an error message to stderr and exit
 * 
 * @param allocReturn The return value of malloc.
 * @param line The line number of the file where the error occurred.
 * 
 * @return allocReturn if it's not null, exit otherwise
 */
void* tryalc(void* allocReturn, int line) {
    if (allocReturn) return allocReturn;

    //*malloc returned null => exiting with error message ENOMEM
    fprintf(stderr, "line %d: Cannot allocate Memory\n", line);
    exit(ENOMEM);
}

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
char* catArr(char** arr, int arrSize, char* sep) {
    char* out;
    int sepLen = strlen(sep);
    size_t size = arrSize + 1 + (sep ? sepLen * arrSize : 0);

    tryalc(out = (char*) malloc((size * sizeof(char*))), __LINE__);

    for (int i = 0; i < arrSize; i++) {
        char* crt = arr[i];
        if (sep && i != 0) strncat(out, sep, sepLen);
        strncat(out, crt, strlen(crt) + 1);
    }
    return out;
}

/**
 * Same as catArr but start at "start" and stop at "stop"
 * i.e. concat stop-start values
 * 
 * @param arr the array of strings to concatenate
 * @param sep the separator to use between each element of the array.
 * @param start index (inclusive) from where to start the concatenation
 * @param stop (exclusive) index from where to stop the concatenation
 * @return 
 */
char* catArrRange(char** arr, char* sep, int start, int stop) {
    int effSize = stop - start; // effective size
    if (effSize < 1) return NULL;
    char* out;
    int sepLen = strlen(sep);
    size_t size = effSize + 1 + (sep ? sepLen * effSize: 0);

    tryalc(out = (char*) malloc((size * sizeof(char*))), __LINE__);

    for (int i = start; i < stop; i++) {
        char* crt = arr[i];
        if (sep && i != start) strncat(out, sep, sepLen);
        strncat(out, crt, strlen(crt) + 1);
    }
    return out;
}

/**
 * @brief Check if there is at least 1 argument, otherwise exits with code 22 (Invalid argument)
 * @param argc same as argc in main
 * @param fileName argv[0]
 */
void checkEnoughArgs(int argc, char* fileName) {
    if (argc <= 1) {
        fprintf(stderr, "Expected at least 1 argument.\tUsage: %s [-f file1 file2 ...] [<text to hash>]\n", fileName);
        errno = EINVAL;
        perror("checkEnoughArgs()");
        exit(EINVAL);
    }
}

/**
 * Handle case where user does not use the -f flag. (i.e. takes every given string and hash them as 1)
 * Takes array of strings from main function and concatenates them into a single string - the string to hash
 * 
 * @param argc the number of arguments passed to the program
 * @param argv the array of arguments passed to the program
 * 
 * @return 0 if success else error code
 */
int parseArgsAsString(int argc, char* argv[]) {
    int strNb = argc - 1, errcode = 0;
    char** strArgs = &argv[1];  //creating a view on argv[1:]
    
    //argv[] got reversed somehow ?
    
    if (tflag == 0) stringToHash = catArr(strArgs, strNb, " "); //* if "-t" was not used
    else if (optind <= 3) stringToHash = catArrRange(strArgs, " ", 2, strNb); //* if "-t" was used and was parsed correctly

    if (!stringToHash || strlen(stringToHash) == 0) errcode = -1;
    return errcode;
}

/**
 * Extract & return a copy of the different file paths given as argument with the "-f" flag. (does a malloc, remember to free the array)
 * 
 * @param argc The number of arguments passed to the program.
 * @param argv The array of arguments given to the program.
 * @param startIdx The index of the first file in argv, (i.e. idx from wich argv[idx:] contains only the files given as argument (and all of them))
 * @param fileAmnt The variable into which store the amount of files given as argument
 * 
 * @return Copied array of file paths
 */
char** extractFilesFromArgv(int argc, char* argv[], int startIdx, int* fileAmnt) {
    int maxFileAmnt = argc - startIdx;  //? max Number of file given as argument 
    char** filePaths; *fileAmnt = 0; 

    tryalc(filePaths = calloc(maxFileAmnt, sizeof(char*)), __LINE__);
    int i = startIdx;
    //* while there is arguments left and we did not hit another option (like -t) 
    while (i < argc && argv[i][0] != '-') {
        filePaths[i - startIdx] = argv[i]; i++;
        *fileAmnt += 1;
    }
    //for (int i = startIdx; i < argc; i++) *(filePaths + (i - startIdx)) = argv[i];

    return filePaths;
}

/**
 * When program was called with -f or -t option.
 * Extracts the file names from the command line arguments and returns them as an array of strings
 * 
 * @param argc Number of arguments passed to the program.
 * @param argv Array of arguments given to the program.
 * @param opt Option that was given as argument
 * @param fileAmnt Variable into which store the amount of given files.
 *                  [ NB: if fileAmnt stays 0 at the end of the call, then 'f' option was not provided ]
 * 
 * @return 0 if success else error code
 */
int parseOptArgs(int argc, char* argv[], int* fileAmnt) {
    int finit = 0, opt; *fileAmnt = 0;
    //* if fileAmnt stays 0 at the end of the call, then 'f' option was not provided

    while ((opt = getopt(argc, argv, getOptString())) != -1) {
        switch (opt) {
            case 'f':
                if (!finit) {
                    filesToHash = extractFilesFromArgv(argc, argv, optind-1, fileAmnt); //? 2 because Number of file given as argument equals argcount - (<Number of available options> + 1) (e.g. 3 => ./prog.out -f file1 file2 file3)
                    finit = 1;  //* Memory should not be allocated more than once.
                }
                break;

            case 't': {
                if (!tflag) {
                    tryalc(hashMethod = malloc(HASH_METH_MAXLEN * sizeof(char)), __LINE__);
                    if (strlen(optarg) <= 1) return EINVAL;
                    hashMethod = strncpy(hashMethod, optarg, HASH_METH_MAXLEN);
                    tflag = 1; 
                }
                break;
            }

            default:
                fprintf(stderr, ERR_MESS, argv[0]);
                char mess[100];
                sprintf(mess, "optind: %d \t argc: %d", optind, argc);
                errno = EINVAL;
                perror(mess);
                return EINVAL;
        }
    }

    return EXIT_SUCCESS;
}

/**
 * Call parseOptArgs to parse given options then if "-f" was not provided, 
 * call parseArgsAsString to interpret all given argument (that are not options) as 1 single string
 * i.e. "-f file1 file2 [-t <hashMethod>]" parses file1, file2 and hashMethod as separate things to hash.
 * and "s1 s2 [-t <hashMethod>]" parses "s1 s2" as 1 string to hash.
 * 
 * @param argc Number of arguments passed to the program.
 * @param argv Array of arguments given to the program.
 * @param givenFilesToHash pointer to string array into which store the parsed files to hash or null if -f was not provided. Does a malloc. Remember to free! (when according option was chosen)
 * @param fileAmnt Variable into which store the amount of given files.
 * @param givenStringToHash pointer to string into which store the parsed string to hash or null if -f was provided. Does a malloc. Remember to free! (when according option was chosen)
 * @return 0 if success else error code
 */
int parseArgs(int argc, char* argv[], char** givenFileToHash[], int* fileAmnt, char* givenStringToHash[]) {
    int errcode = parseOptArgs(argc, argv, fileAmnt);
    if (errcode) return errcode;

    if (!(*fileAmnt)) {
        errcode = parseArgsAsString(argc, argv);
        if (errcode) {
            fprintf(stderr, "Error %d in parseArgsAsString()\n", errcode);
            return errcode;
        }
        *givenStringToHash = stringToHash;
    } else if (filesToHash) *givenFileToHash = filesToHash;
    //* Make givenFileToHash point to the argument passed here (the option parser). i.e. "redirect" its content to filesToHash

    else errcode = EXIT_FAILURE;
    return errcode;
}

/** @return hashMethod parsed by the optionParser or the defaultHashMethod if none were given in argv (i.e. hahsMethod == NULL) */
char* getHashMethod(char* defaultHashMethod) { return hashMethod ? hashMethod : defaultHashMethod; }
