#include <ctype.h>  // isBlank
#include <errno.h>
#include <limits.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

#include "input.h"
#include "util.h"

const char EOL = '\n';
const char* ARG_SEP = " ";  //Tab and space are the only argument separator
// 0:default,  1:red, 2:Green,  3:Blue, 4:Purple, 5:yellow,  6:cyan,  7:grey
const char* colors[] = {"\033[0m", "\033[0;31m", "\033[0;32m", "\033[0;34m", "\033[0;35m", "\033[0;33m", "\033[0;36m", "\033[0;30m"};

//* symbol present at start every "input line" in shell (like the '$' )
const char* prompt = "\033[0;31m|_ \033[0;33m$\033[0m ";
//snprintf(msg, 24, "%s|_ %s$ %s ", colors[1], colors[5], colors[0]);

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

const char* readInput() {
    const char* buff = readline(prompt);
    /*
     * " readline  returns  the  text of the line read.  A blank line returns the empty string.  If EOF is encountered
     *   while reading a line, and the line is empty, NULL is returned.  If an EOF is read with a non-empty line, it is
     *   treated as a newline. "
     */
    if (buff == NULL) {
        //EOF was encountered
        // => send SIGUSR1 that will be "caught" in Shell.c to terminate its child processes and exit cleanly
        if (kill(0, SIGUSR1) < 0) 
            printErr("readInput: cannot inform shell.c : %s (EOF received) %s\n", buff);
        return NULL;
    }
    if (strlen(buff) <= 0) return NULL;
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
    int isFg; size_t argcTmp = 0, inp_len = strlen(inp);
    // inp is already right stripped so if required job must be launched in background, the '&' has to be end at the last position of 'inp'
    char* tosplit = strndup(inp, inp_len); // no +1 because strndup will add a '\0' at the end whether there is already one or not, + it might be good in some case to do a "safe" copy of the arg to avoid "effet de bords" //tryalc(malloc(1));

    if (inp_len > 1 && inp[inp_len - 1] == '&') {
        isFg = 0;
        tosplit[inp_len - 2] = '\0';  //if there is a '&' then there is a space at the end
        tosplit = stripr(tosplit);
    } else
        isFg = 1;

    const char** argv = strsplit(tosplit, ARG_SEP, &argcTmp);
    free(tosplit);

    *isForeground = isFg;
    *argc = (int)argcTmp;
    return argv;
}

const char** readParseIn(int* argc, int* isForeground) {
    const char* tmp = readInput();
    //printf("%s", colors[0]); //reset colors because it seems to stay stuck in red.
    if (tmp == NULL) return NULL;
    add_history(tmp);
    const char** parsedInput = parseInput(tmp, argc, isForeground);
    free(tmp); // add_history seems to make a copy of tmp
    return parsedInput;
}

const char* getPrompt() {
    return prompt;
}
