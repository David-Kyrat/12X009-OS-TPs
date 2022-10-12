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
    //errno = ENOMEM;
    fprintf(stderr, "line %d: Cannot allocate Memory\n", line);
    exit(ENOMEM);
}

char* catArr(char** arr, int arrSize, char* sep) {
    char* out;
    int sepLen = strlen(sep);
    size_t size = arrSize + 1 + (sep ? sepLen * arrSize : 0);

    tryalc(out = (char*)malloc((size * sizeof(char*))), __LINE__);

    for (int i = 0; i < arrSize; i++) {
        char* crt = arr[i];

        if (sep) strncat(out, sep, sepLen);
        strncat(out, crt, strlen(crt) + 1);
    }

    return out;
}

int main(int argc, char* argv[]) {
    int fileGiven = 0, opt;
    tryalc(NULL, __LINE__);

    char** filePaths;  //array of path to the given files

    printf("argc=%i\t argv=[", argc);
    for (int i = 0; i < argc - 1; i++) printf("\"%s\",  ", argv[i]);
    printf("\"%s\"]\n\n", argv[argc - 1]);

    if (optind <= 1) {
        //TODO: treat every arg as a single
    }

    while ((opt = getopt(argc, argv, availableOptions)) != -1) {
        switch (opt) {
            printf("optind=%i\n", optind);
            case 'f':
                fileGiven = 1;
                int fileAmnt = argc - 2;  //? Number of file given as argument equals argcount - (<Number of available options> + 1) (e.g. 3 => ./prog.out -f file1 file2 file3)
                break;

            default: /* '?' */
                fprintf(stderr, errMess, argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    printf("fileGiven=%d\t optind=%d\n\n", fileGiven, optind);

    if (argc <= 1) {
        //printf("test\n");
        fprintf(stderr, "Expected at least 1 argument,\n\tUsage: %s [-f file1 file2 ...] [<text to hash>]\n", argv[0]);

        //perror(optarg);
        errno = EINVAL;
        /* exit(EXIT_FAILURE);
        fprintf(stderr, "Expected argument after options\n"); */
        perror(optarg);

        return EINVAL;
    }

    printf("last argument:\"%s\"\n", argv[argc - 1]);
    exit(EXIT_SUCCESS);
}

