#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include "OptionParser.h"


int main(int argc, char* argv[]) {
    checkEnoughArgs(argc, argv[0]);

    char opt;  //* We expect only 1 options (-f), no need to have a loop.
    if ((opt = getopt(argc, argv, getOptString())) != -1) {
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
        char* stringToHash = parseSingleArg(argc, argv);

        //TODO: Call hash_calc on stringToHash

        free(stringToHash);
    }

    exit(EXIT_SUCCESS);
}
