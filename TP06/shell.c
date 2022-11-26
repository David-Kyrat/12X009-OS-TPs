#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#include "shell.h"
#include "util.h"

// TODO: Create a struct with a local copy of pwd (the current path) that will be udpated on cd
// (TO avoid calling getwd function that fills a buffer over and over again for each time user enters something)
int update_path();

const char crt_path[PATH_MAX];

void sh_init() { update_path(); }

void printPid(int in) {
    char* msg = in ? "PID=%d> " : "[PID=%d] ";
    printf(msg, getpid());
}


void prettyPrintPath() {
    printf("( %s )\n|_ $ ", crt_path);
}

int cd(const char* path) {
    if (chdir(path) < 0) printRErr("cd : %s - %s\n", path);
    // if successfully changed path:
    if (update_path() < 0) return -1;

    return EXIT_SUCCESS;
}


int update_path() {
     if (getcwd(crt_path, PATH_MAX) < 0)
        printRErr("%s - cannot update pwd (%s)\n", crt_path);
     
     return EXIT_SUCCESS;
}



const char* pwd() {
    return crt_path;
}