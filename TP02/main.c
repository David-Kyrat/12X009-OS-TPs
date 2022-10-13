#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "hash_calc.h"
#include "OptionParser.h"

const char* DEFAULT_HASH_METH = "SHA1";


int main(int argc, char* argv[]) {
    checkEnoughArgs(argc, argv[0]);
    char** filesToHash = NULL;
    char* stringToHash = NULL;
    int fileAmnt = -1;

    int parseError = parseArgs(argc, argv, &filesToHash, &fileAmnt, &stringToHash);
    if (parseError != 0) {
        fprintf(stderr, "Error %d in parseArgs()\n", parseError);
        exit(parseError);
    }
    char* hashMethod = getHashMethod(DEFAULT_HASH_METH);
    printf("fileAmnt %d\n", fileAmnt);
    printf("String to hash: %s\n", stringToHash);


    if (filesToHash) { //* if the array of files is not null, i.e. has been correctly initialized
        char** fileHashs;
        tryalc(fileHashs = (char**)calloc(fileAmnt,  sizeof(char*)), __LINE__);
        //for (int i = 0; i < fileAmnt; i++) fileHashs[i] = "0x"; //? Debugging purposes - TOREMOVE

        for (int i = 0; i < fileAmnt; i++) {
            char* crtFile = filesToHash[i];
            printf("Hashing file \"%s\"...\n", crtFile);

            //fileHashs[i] =
            //TODO: Call hash_calc with hashMethod on current file

            //printf("fileHash[%i]: \t \"%s\"\n\n", i, fileHashs[i]); //? Debugging purposes - TOREMOVE
        }

        free(fileHashs);
    }
    else {

        printf("String to hash: %s\n", stringToHash);
        //TODO: Call hash_calc with hashMethod on stringToHash

        free(stringToHash);        

    }

     exit(EXIT_SUCCESS);
   
}
