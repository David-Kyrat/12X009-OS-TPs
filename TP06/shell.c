#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#include "shell.h"
#include "input.h"
#include "util.h"

#define CMD_CD ("cd")
#define CMD_EXIT ("exit")

const int CMD_NB = 2;
const char* CMDS[CMD_NB] = {CMD_CD, CMD_EXIT}; 

// TODO: Create a struct with a local copy of pwd (the current path) that will be udpated on cd
// (TO avoid calling getwd function that fills a buffer over and over again for each time user enters something)
int update_path();

// copy of pwd environment variable as a field, to not having to refetch it everytime
const char crt_path[PATH_MAX]; 

void sh_init() { update_path(); }


/**
 * Changes the current working directory to the one specified by the path argument
 * @param path The path to the directory to change to.
 * @return Exit code. 0 for success, -1 for error.
 */
int cd(const char* path) {
    if (chdir(path) < 0) printRErr("cd : %s - %s\n", path);
    // if successfully changed path:
    if (update_path() < 0) return -1;

    return EXIT_SUCCESS;
}


/**
 * Update the 'crt_path' field to actual current working directory (the one returned by 'getcwd()')
 * @return Exit cod. 0 if success -1 otherwise.
 */
int update_path() {
     if (getcwd(crt_path, PATH_MAX) == NULL)
        printRErr("%s - cannot update pwd (%s)\n", crt_path);
     
     return EXIT_SUCCESS;
}


const char* pwd() {
    return crt_path;
}


/**
 * Exit shell with given exitcode
 * @param arg exitcode as string
 */
void exit_shell(char* arg) {
    int exit_code;
    int err = strToInt(arg, 10, exit_code);
    // if conversion failed, only change exit_code if there was no error before (i.e. exit_code == "EXIT_SUCCESS")
    if (err < 0 && exit_code == EXIT_SUCCESS) exit_code = EXIT_FAILURE;
    printf("Exit with exit code %d\n", exit_code);
    exit(exit_code);
}


int getAndResolveCmd() {
    int argc; 
    const char** argv = readParseIn(&argc);
    if (argc == 0 || argv == NULL)
        printRErr("%s: Could not parse user input - %d argument entered\n", argc);
    
    const char* cmd_name = argv[0];
    //const char* cmd_hash = hash(cmd_name);
    switch (strswitch(cmd_name, CMDS, CMD_NB)) {
        case 0: 
            // CMDS[0] is "cd". => cd to 2nd argument in argv ignoring the rest.
            if (cd(argv[1]) < 0)  return -1;
            break;
        case 1:
            // CMDS[1] is "exit"
            exit_shell(argv[1]);
            //? break here is useless since we exit, but do we have to add it for good practices ?

        default:
            // execve to execute program requested by user
            //TODO: implement execve call

            break;
    }

    return EXIT_SUCCESS;
}



// ================ Printing functions ====================

/* void printPid(int in) {
    char* msg = in ? "PID=%d> " : "[PID=%d] ";
        printf(msg, getpid());
} */


void prettyPrintPath() {
    printf("( %s )\n|_ $ ", crt_path);
}
