#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <ctype.h>

#include "shell.h"
#include "util.h"
#include "input.h"
#include "files.h"

// Types
#include <sys/types.h>

#define printExitCode(exitcode) \
    printf("%s  -Foreground job exited with exit code %d\033[0m\n\n", "\033[2m", exitcode);

#define CMD_CD ("cd")
#define CMD_EXIT ("exit")


const int CMD_NB = 2;
const char* CMDS[] = {CMD_CD, CMD_EXIT}; 

// TODO: Create a struct with a local copy of pwd (the current path) that will be udpated on cd
// (TO avoid calling getwd function that fills a buffer over and over again for each time user enters something)
int update_path();

// copy of pwd environment variable as a field, to not having to refetch it everytime
char crt_path[PATH_MAX];

extern char** environ;

void sh_init() { update_path(); }


/**
 * Changes the current working directory to the one specified by the path argument
 * @param path The path (relative or absolute) to the directory to change to.
 * @return Exit code. 0 for success, -1 for error.
 */
int cd(const char* path) {
    char* resolvedPath;
    if (!path)  resolvedPath = getenv("HOME");
    else {
        int isAbsolute = (*path == '/');
        if (isAbsolute) resolvedPath = path; 
        else {
            const char* tmp = concat_path(crt_path, path);
            resolvedPath = absPath(tmp);
            free(tmp);
            if (resolvedPath == NULL) return -1;
        } 
    }

    //const char* asb_path = absPath(path);

    /*if (strcmp(path, "..") == 0) {
        chdir("..");
        if (update_path() < 0) return -1;
        return EXIT_SUCCESS;
    }*/


    if (chdir(resolvedPath) < 0) {
        printRErr("cd : %s - %s\n", path);
    }
    else if (update_path() < 0) return -1;

    // if successfully changed path:
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
void exit_shell(const char* arg) {
    int exit_code;
    if (!arg) exit_code = 0;
    else {
        int err = strToInt(arg, 10, &exit_code);
        // if conversion failed, only change exit_code if there was no error before (i.e. exit_code == "EXIT_SUCCESS")
        if (err < 0 && exit_code == EXIT_SUCCESS) exit_code = EXIT_FAILURE;
    }
    printf("Exit with exit code %d\n\n", exit_code);
    exit(exit_code);
}

/** Utility function if we ever need to do something to environ (e.g. pretreat it...) before returing it*/
char** getEnvp() {
    return environ;
}

void listEnv() {
    for (char **current = environ; *current; current++) puts(*current);
}

/**
 * Call execvpe, handle errors and print "Foreground job exited with exit code <errorcode extracted when handling error>" 
 */
int exec(const char* filename, char *const argv[]) {
    errno = 0; int exitcode = 0;
    if (execvpe(filename, argv, getEnvp()) < 0) {
        exitcode = errno;
        fprintf(stderr, "%s: \"%s\" \n", exitcode == ENOENT ? "command not found" : strerror(exitcode), filename);
    }
    
    return -1;
    // if exec command returns then there have been an error somewhere
}

int executeJob(const char* cmd_name, char *const argv[]) {
    if (cmd_name == NULL || strlen(cmd_name) <= 0) return -1;
            pid_t t_pid = fork();
            int child_exitcode = EXIT_FAILURE;

            if (t_pid < 0) {
                printErr("%s: %s - Cannot Fork.\n", argv[0]);
                //printf("Foreground job exited\n\n");
                //exit(EXIT_FAILURE);
                printExitCode(child_exitcode);
                return -1;
            }

            //* In parent 
            if (t_pid > 0) wait(&child_exitcode);

            if (t_pid == 0) {
                //* In child_number
                if (exec(cmd_name, argv) < 0) 
                        exit(EXIT_FAILURE);
                // if it is not programm that was called shouldve exited
            }
            printExitCode(child_exitcode);
            //printf("%s  -Foreground job exited with exit code %d%s\n\n", colors[7], child_exitcode,  colors[0]);
            return EXIT_SUCCESS;
}


int sh_getAndResolveCmd() {
    //TODO: Check for & => and make background job
    int argc; 
    const char** argv = readParseIn(&argc);
    if (argc <= 0 || argv == NULL)
        printRErr("%s: Could not parse user input - %d argument entered\n", argc); //returns -1
    const char* cmd_name = argv[0];
    switch (strswitch(cmd_name, CMDS, CMD_NB)) {
        
        case 0: 
            // CMDS[0] is "cd". => cd to 2nd argument in argv ignoring the rest.
            if (cd(argc <= 1 ? NULL : argv[1]) < 0)  return -1;
            printf("\n");
            break;
        case 1:
            // CMDS[1] is "exit"
            // if no exit code 2nd arg is blank => exit with default code
            exit_shell((argc <= 1 || *argv[1] == '\n') ? NULL : argv[1]);

            //? break here is useless since we exit, but do we have to add it for good practices ?
            // answer: no.

        default: ;
            // execve to execute program requested by user
                 //const char* const* args = argv; 
                 if (executeJob(cmd_name, argv) < 0) return -1; 

    }

    return EXIT_SUCCESS;
}



// ================ Printing functions ====================

/* void printPid(int in) {
    char* msg = in ? "PID=%d> " : "[PID=%d] ";
        printf(msg, getpid());
} */


//! 0:default,  1:red, 2:Green,  3:Blue, 4:Purple, 5:yellow,  6:cyan,  7:grey
const char* colors[] = {"\033[0m", "\033[0;31m", "\033[0;32m", "\033[0;34m", "\033[0;35m", "\033[0;33m", "\033[0;36m", "\033[2m"};
/**
 * 0:default,  1:red, 2:Green,  3:Blue, 4:Purple, 5:yellow,  6:cyan
 */
void setOutColor(int color) {
    printf("%s", colors[color]);
}
void resetCol() {
	setOutColor(0);
}
void sh_prettyPrintPath() {
    printf("%s( %s", colors[1], colors[6]);
    printf("%s", crt_path);
    printf("%s )\n|_ ", colors[1]);
    printf("%s$ ", colors[5]);
    resetCol();
}
