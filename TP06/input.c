#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h> // isBlank
#include <readline/readline.h>
#include <readline/history.h>
#include "input.h"
#include "util.h"

const char EOL = '\n';
const char* ARG_SEP = " ";  //Tab and space are the only argument separator
//!
//! Below is the basic code of readInput() (function that returns what the user entered) which I preferred to replace by the GNU library "readline.h" by simplicity. The library just adds more feature (like going back and forward) when User has to enter text. Everything else is still made by us, it is just that specific small  part that was replaced for convenience.
//! 
//! The basic function below of course works, feel free to uncomment this one and comment to the other (and remove the <readline/*> includes) to test it
//!
//! If you get an error message saying, that the file was not found, you can install it with
//! "sudo apt-get install lib32readline8 lib32readline-dev" and "sudo apt-get install libreadline-dev"
//!

/*const char* readInput() {
    printf("%s|_ %s$ %s ", colors[1], colors[5], colors[0]);
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
    return strip(buff);
}*/

// 0:default,  1:red, 2:Green,  3:Blue, 4:Purple, 5:yellow,  6:cyan,  7:grey
const char* colors[] = {"\033[0m", "\033[0;31m", "\033[0;32m", "\033[0;34m", "\033[0;35m", "\033[0;33m", "\033[0;36m","\033[2m"};

const char* readInput() {
    char msg[25];
    snprintf(msg, 24, "%s|_ %s$ %s ", colors[1], colors[5], colors[0]);
    const char* buff = readline(msg);
    strlen(buff);
    if (buff == NULL || strlen(buff) <= 0) return  NULL;
    return strip(buff);
    
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

/**
 * Split given input (with respect to 'ARG_SEP') into an argv array 
 * and stores the argc into the given 'argc' variable.
 * 
 * @param inp raw user input
 * @param argc ptr to variable in whcih to store the number of argument
 * @return string buffer (usually argv)  i.e. 'inp' split with respect to 'ARG_SEP'
 */
const char** parseInput(const char* inp, int* argc, int* isForeground) {
    uint isFg; size_t argcTmp = 0, inp_len;
    // inp is already right stripped so if required job must be launched in background, the '&' has to be end at the last position of 'inp'
    char* tosplit = tryalc(malloc(1));
    tosplit = inp; inp_len = strlen(inp);

    if (inp_len > 1 && inp[inp_len - 1] == '&') {
        isFg = 0;
        tosplit[inp_len - 2] = '\0'; //if there is a '&' then there is a space at the end
        tosplit = stripr(tosplit);
    } else isFg = 1;
    
    const char** argv = strsplit(tosplit, ARG_SEP, &argcTmp);
    free(tosplit);

    *isForeground = isFg; *argc = (int) argcTmp;
    return argv;
}


const char** readParseIn(int* argc, int* isForeground) {
    const char* tmp = readInput();
    //printf("%s", colors[0]); //reset colors because it seems to stay stuck in red.
    if (tmp == NULL) return NULL;
    return parseInput(tmp, argc, isForeground);
}
