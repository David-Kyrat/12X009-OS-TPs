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
        exit(EXIT_FAILURE);
    }
}

//TODO: wrap everything in method parseArgs

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

//!
//! The main will not be here !
//!

int main(int argc, char* argv[]) {
    checkEnoughArgs(argc, argv[0]);

    printf("argc=%i\t argv=[%s]\n", argc, catArr(argv, argc, ", "));
    printf("optind=%i\n", optind);

    int fileGiven = 0, opt;
    char** filePaths;
    
    while ((opt = getopt(argc, argv, availableOptions)) != -1) {
        printf("optind=%i\t opt=%c\n", optind, opt);

        switch (opt) {
            case 'f':
                if (optind <= 1) {
                    fileGiven = 1;
                }
                
                //filePath;          //array of path to the given files
                int fileAmnt = argc - 2;  //? Number of file given as argument equals argcount - (<Number of available options> + 1) (e.g. 3 => ./prog.out -f file1 file2 file3)
                break;

            default: /* '?' */
                fprintf(stderr, errMess, argv[0]);
                exit(EXIT_FAILURE);
        } 
        //* If flag is something else than 'f' with arguments, the program just exits. so we don't have to handle multiple case and branching 
        //* because after having entered the while if the programm didnt exit then the arguments must have been parsed correctly.

    }
    printf("optind=%i\n", optind);

    
    if (optind <= 1) {
        char* stringToHash = parseSingleArg(argc, argv);
        
        //TODO: Call hash_calc on stringToHash

        free(stringToHash);
        return EXIT_SUCCESS;
    }
   


    exit(EXIT_SUCCESS);
}
