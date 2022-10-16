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
    printf("argv: [%s]\n", catArr(argv, argc, ", "));


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


    // If files are given, do this part

    if (filesToHash) { //* if the array of files is not null, i.e. program was called with -f option and has been correctly initialized
        char** fileHashs;
        tryalc(fileHashs = (char**)calloc(fileAmnt,  sizeof(char*)), __LINE__);
        //for (int i = 0; i < fileAmnt; i++) fileHashs[i] = "0x"; //? Debugging purposes - TOREMOVE

        for (int i = 0; i < fileAmnt; i++) {
            char* crtFile = filesToHash[i];
            printf("Hashing file \"%s\"...\n", crtFile);

            
            //TODO: Call hash_calc with hashMethod on current file
            fileHashs[i] = hash(crtFile, hashMethod);

            printf("Digest: %s, File: %s\n", fileHashs[i], crtFile);
            //printf("fileHash[%i]: \t \"%s\"\n\n", i, fileHashs[i]); //? Debugging purposes - TOREMOVE
        }

        free(fileHashs);
    }

    // If a string is given, do this part

    else {
        printf("String to hash: \"%s\"\n", stringToHash);
        //TODO: Call hash_calc with hashMethod on stringToHash
        printf("Here");
        // printf("Digest: %s\n", hash(stringToHash, hashMethod));

        free(stringToHash);        

    }

     exit(EXIT_SUCCESS);
   
}
