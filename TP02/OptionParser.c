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

//TODO: wrap everything in method parseArgs

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        fprintf(stderr, "Expected at least 1 argument.\tUsage: %s [-f file1 file2 ...] [<text to hash>]\n", argv[0]);
        errno = EINVAL;
        exit(EXIT_FAILURE);
    }

    int fileGiven = 0, opt;

    //printf("argc=%i\t argv=[%s]\n", argc, catArr(argv, argc, ", "));

    if (optind <= 1) {
        int strNb = argc - 1;
        char** strArgs = &argv[1];  //creating a view on argv[1:]
        char* stringToHash = catArr(strArgs, strNb, " ");

        printf("String to hash:\t\t\"%s\"\n", stringToHash);
        //TODO: Call hash_calc on stringToHash

        
        free(stringToHash);
        return EXIT_SUCCESS;
    }

    while ((opt = getopt(argc, argv, availableOptions)) != -1) {
        printf("optind=%i\n", optind);
        
        switch (opt) {
            case 'f':
                fileGiven = 1;
                char** filePath;     //array of path to the given files
                int fileAmnt = argc - 2;  //? Number of file given as argument equals argcount - (<Number of available options> + 1) (e.g. 3 => ./prog.out -f file1 file2 file3)
                break;

            default: /* '?' */
                fprintf(stderr, errMess, argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    printf("fileGiven=%d\t optind=%d\n\n", fileGiven, optind);

    printf("last argument:\"%s\"\n", argv[argc - 1]);
    exit(EXIT_SUCCESS);
}
