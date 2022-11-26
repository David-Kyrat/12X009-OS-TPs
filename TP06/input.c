#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>


#include "util.h"

const char EOL = '\n';


char gett() {
    int tmp = getchar();
    char out = ((char) tmp);
    return out;
}

const char* readInput() {
    int crt_size = MAX_INPUT;
    char* buff = tryalc(malloc(crt_size + 1));
    memset(buff, 0, crt_size);

    ssize_t readNb = 0;
    //size_t readTotNb = 0;
    
    // while there are still character to be read on stdin
    while((buff[readNb++] = getchar()) != EOF) {
        //* If there is no space left in buffer to store user input => realloc more memory it (double its size) (same concept as a regular ArrayList)
        if (readNb >= crt_size-1) {
            crt_size *= 2;
            buff = realloc(buff, crt_size + 1); 
        }
    }

    
    return buff;
}