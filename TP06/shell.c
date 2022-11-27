#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#include "shell.h"
#include "input.h"
#include "util.h"

/* const char CMDS[] = {
    hash("cd"),
    hash("exit")
}; */

// TODO: Create a struct with a local copy of pwd (the current path) that will be udpated on cd
// (TO avoid calling getwd function that fills a buffer over and over again for each time user enters something)
int update_path();


const char crt_path[PATH_MAX];

void sh_init() { update_path(); }


int cd(const char* path) {
    if (chdir(path) < 0) printRErr("cd : %s - %s\n", path);
    // if successfully changed path:
    if (update_path() < 0) return -1;

    return EXIT_SUCCESS;
}


int update_path() {
     if (getcwd(crt_path, PATH_MAX) == NULL)
        printRErr("%s - cannot update pwd (%s)\n", crt_path);
     
     return EXIT_SUCCESS;
}


const char* pwd() {
    return crt_path;
}


int getAndResolveCmd() {
    int argc; 
    const char** argv = readParseIn(&argc);
    if (argc == 0 || argv == NULL)
        printRErr("%s: Could not parse user input - %d argument entered\n", argc);
    
    const char* cmd_name = argv[0];
    const char* cmd_hash = hash(cmd_name);



    if (argc == 1) {
        const char* arg = argv[0];
        // Hashs are supposed to be easy&fast to compute

    }

}



// ================ Printing functions ====================

void printPid(int in) {
    char* msg = in ? "PID=%d> " : "[PID=%d] ";
        printf(msg, getpid());
}


void prettyPrintPath() {
    printf("( %s )\n|_ $ ", crt_path);
}
