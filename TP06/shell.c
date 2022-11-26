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

const char crt_path[PATH_MAX];

void printPid(int in) {
    char* msg = in ? "PID=%d> " : "[PID=%d] ";
    printf(msg, getpid());
}

void printpath() {
    printf(" - ( %s )\n|_ $ ", pwd(crt_path));
}


int cd(const char* path) {
    if (chdir(path) < 0) printRErr("cd : %s - %s\n", path);
    // if successfully changed path:

    return EXIT_SUCCESS;
}


const char* pwd() {
    return getwd(crt_path);
}