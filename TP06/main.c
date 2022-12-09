#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "shell.h"

int main(int argc, char* argv[]) {
    printf("\n");
    int err = EXIT_SUCCESS;
    Shell* shell = new_Shell();

    // wrapper for manage signals that has access to a shell instance in context  (we can't pass it as argument 
    // so this is the only way of accessing an instance of 'Shell' which is not a global variable)
    void manage_signals_wrapper(int signum, siginfo_t* info, void* ucontext) { manage_signals(signum, info, shell); }

    if (initSigHandlers(shell, &manage_signals_wrapper) < 0) 
        exit(EXIT_FAILURE);

    for (;;) {


        sh_prettyPrintPath(shell);
        
        int tmp = sh_getAndResolveCmd(shell);
        if (tmp < 0 ) {
            err = EXIT_FAILURE;
            fprintf(stderr, "Command could not be resolved. Please try again.\n\n");
        }
        // Sometimes errors stays stuck on stderr which prevents programm from displaying
        // what should be in stdout correctly
        fflush(stderr);
        fflush(stdout);
    }

    return err;
}
