#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "hash_calc.h"
#include "OptionParser.h"

const char* DEFAULT_HASH_METH = "SHA1";


int main(int argc, char* argv[]) {

    // Check that all the arguments are given.
    checkEnoughArgs(argc, argv[0]);
    char** filesToHash = NULL;
    char* stringToHash = NULL;
    int fileAmnt = -1;


    // Parse the arguments
    int parseError = parseArgs(argc, argv, &filesToHash, &fileAmnt, &stringToHash);
    if (parseError != 0) {
        fprintf(stderr, "Error %d in parseArgs()\n", parseError);
        errno = parseError; perror("parseArgs()");
        exit(parseError);
    }

    // Set the hash method as default (SHA1) if one isn't given. If it is given, set the hash method as it.
    char* hashMethod = getHashMethod(DEFAULT_HASH_METH);
    printf("Hashing Method: %s\n", hashMethod);


    if (filesToHash) { //* if the array of files is not null, i.e. program was called with -f option and has been correctly initialized
        unsigned char** fileHashs;
        fileHashs = (unsigned char**) tryalc(calloc(fileAmnt,  sizeof(unsigned char*)));
        //for (int i = 0; i < fileAmnt; i++) fileHashs[i] = "0x"; //? Debugging purposes - TOREMOVE

        for (int i = 0; i < fileAmnt; i++) {
            char* crtFile = filesToHash[i];
            printf("Hashing file \"%s\"...\n", crtFile);

            fileHashs[i] = hash(convert_f_to_s(crtFile), hashMethod);
            printf(" File: %s\n", crtFile);
            printf("____________________________________________\n\n");
        }

        free(fileHashs);
    }

    // If a string is given, do this part

    else {
        printf("String to hash: \"%s\"\n", stringToHash);
        
        printf("Digest: ");
        hash(stringToHash, hashMethod);
        //printf("%s\n", hash(stringToHash, hashMethod));
        printf("\n");
        free(stringToHash);        

    }

     exit(EXIT_SUCCESS);
}
