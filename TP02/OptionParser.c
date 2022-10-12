#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const char* errMess = "\tUsage: %s [-f file1 file2 ...] [<text to hash>]\n";
const char* availableOptions = "f:";

/**
 * If malloc returns null, print an error message and exit
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

    tryalc(out = (char*)malloc((size * sizeof(char*))), __LINE__);

    for (int i = 0; i < arrSize; i++) {
        char* crt = arr[i];
        if (sep && i != 0) strncat(out, sep, sepLen);
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
 * @return The String to hash
 */
char* parseSingleArg(int argc, char* argv[]) {
    int strNb = argc - 1;
    char** strArgs = &argv[1];  //creating a view on argv[1:]
    char* stringToHash = catArr(strArgs, strNb, " ");
    printf("String to hash:\t\t\"%s\"\n", stringToHash);

    return stringToHash;
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
    *fileAmnt = argc - startIdx;  //? Number of file given as argument equals argcount - (<Number of available options> + 1) (e.g. 3 => ./prog.out -f file1 file2 file3)
    char** filePaths;

    tryalc(filePaths = calloc(*fileAmnt, sizeof(char*)), __LINE__);
    for (int i = startIdx; i < argc; i++) *(filePaths + i) = argv[i];

    return filePaths;
}

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
char** parseOptArgs(int argc, char* argv[], char opt, int* fileAmnt) {
    char** filesToHash = NULL;
    switch (opt) {
        case 'f':
            filesToHash = extractFilesFromArgv(argc, argv, 2, fileAmnt);
            //? 2 because Number of file given as argument equals argcount - (<Number of available options> + 1) (e.g. 3 => ./prog.out -f file1 file2 file3)
            break;

        default:
            fprintf(stderr, errMess, argv[0]);
            exit(EXIT_FAILURE);
            break;
    }

    return filesToHash;
}

//!
//! The main will not be here !
//!

int main(int argc, char* argv[]) {
    checkEnoughArgs(argc, argv[0]);
    //printf("argc=%i\t argv=[%s]\n", argc, catArr(argv, argc, ", "));
    //printf("optind=%i, argv[optind]=%s\n", optind, argv[optind]);

    char opt;  //* We expect only 1 options (-f), no need to have a loop.
    if ((opt = getopt(argc, argv, availableOptions)) != -1) {
        printf("optind=%i, argv[optind]=%s\t opt=%c\n", optind, argv[optind], opt);

        int fileAmnt = 0;
        char** filePaths = parseOptArgs(argc, argv, opt, &fileAmnt);
        char** fileHashs;
        tryalc(fileHashs = calloc(fileAmnt, sizeof(char*)), __LINE__);

        for (int i = 0; i < fileAmnt; i++) {
            char* crtFile = filePaths[i];
            
            printf("Hashing file \"%s\"\n", crtFile);
            //fileHashs[i] = 
            //TODO: Call hash_calc on current file
            
        }

        free(filePaths);
        free(argv);

    } else {
        char* stringToHash = parseSingleArg(argc, argv);

        //TODO: Call hash_calc on stringToHash

        free(stringToHash);
        return EXIT_SUCCESS;
    }

    printf("optind=%i\n", optind);

    exit(EXIT_SUCCESS);
}
