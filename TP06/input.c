#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h> // isBlank

#include "input.h"
#include "util.h"

const char EOL = '\n', ARG_SEP = ' ';  //Tab and space are the only argument separator

/**
 * 
 * Reads user input from stdin and returns it as a string
 * 
 * @return User input or null if nothing could be read
 */
const char* readInput() {
    int crt_size = MAX_INPUT;
    char* buff = tryalc(malloc(crt_size + 1));
    memset(buff, 0, crt_size);
    ssize_t readNb = 0;

    // while there are still character to be read on stdin
    while ((buff[readNb++] = getchar()) != EOL) {
        //* If there is no space left in buffer to store user input => realloc more memory it (double its size) (same concept as a regular ArrayList)
        if (readNb >= crt_size - 1) {
            crt_size *= 2;
            buff = realloc(buff, crt_size + 1);
        }
    }
    if (readNb <= 0) {
        hdlReadInErr(0);
        return NULL;
    }
    buff[readNb] = '\0';  //Null terminate string in case anything happened
    return buff;
}

/**
 * Checks if each char in 'str' is a space or a tab
 * @param str string to check
 * @return 1 if true 0 otherwise
 */
int isWhiteSpace(const char* str) {
    size_t len = strlen(str);
    for (size_t i = 0; i < len; i++) {
        if (!isblank(str[i])) return 0;
    }
    return 1;
}

const char** parseInput(const char* inp, int* argc) {
    //TODO:
    /* if (strncmp(inp, "exit", 5) == 0) {
        exit(inp[6]);
    } */
    char** argv = strsplit(inp, ARG_SEP, argc);


    return argv;
}

/**
 * Reads user input from stdin and then parses it (if it was correctly read)
 * 
 * @return Parsed input as a string 
 */
const char* readParseIn() {
    const char* tmp = readInput();
    if (tmp == NULL) return NULL;
    int argc = 0;
    return parseInput(tmp, &argc);
}
