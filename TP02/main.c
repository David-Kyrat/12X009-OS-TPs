#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

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

    if (filesToHash) { //* if the array of files is not null, i.e. has been correctly initialized
        char** fileHashs;
        tryalc(fileHashs = (char**)calloc(fileAmnt,  sizeof(char*)), __LINE__);
        for (int i = 0; i < fileAmnt; i++) fileHashs[i] = "0x"; //? Debugging purposes - TOREMOVE

        for (int i = 0; i < fileAmnt; i++) {
            char* crtFile = filesToHash[i];
            printf("Hashing file \"%s\"...\n", crtFile);

            //fileHashs[i] =
            //TODO: Call hash_calc with hashMethod on current file

            printf("fileHash[%i]: \t \"%s\"\n\n", i, fileHashs[i]); //? Debugging purposes - TOREMOVE
        }

        free(fileHashs);
    }
    else {

        //TODO: Call hash_calc with hashMethod on stringToHash

        free(stringToHash);        

    }


     exit(EXIT_SUCCESS);
    /* if ((opt = getopt(argc, argv, getOptString())) != -1) {
        //printf("optind=%i\t argv[optind]=%s\t opt=%c\n", optind, argv[optind], opt);

        int fileAmnt = 0;
        char** filePaths = parseOptArgs(argc, argv, opt, &fileAmnt);
        
        char** fileHashs;
        tryalc(fileHashs = (char**)malloc(((fileAmnt+1) * sizeof(char*))), __LINE__);  
        //for (int i = 0; i < fileAmnt; i++) fileHashs[i] = "0x"; //? Debugging purposes - TOREMOVE

        for (int i = 0; i < fileAmnt; i++) {
            char* crtFile = filePaths[i];
            printf("Hashing file \"%s\"...\n", crtFile);
            
            //fileHashs[i] = 
            //TODO: Call hash_calc on current file

            //printf("fileHash[%i]: \t \"%s\"\n\n", i, fileHashs[i]); //? Debugging purposes - TOREMOVE
        }

        free(filePaths);
    } else {
        char* stringToHash = parseArgsAsString(argc, argv);

        //TODO: Call hash_calc on stringToHash

        free(stringToHash);
    } */

   
}
