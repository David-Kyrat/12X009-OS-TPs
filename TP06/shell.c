//!
//! Temporary file for the struct shell. => Made another one to keep a clean working one
//!

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <unistd.h>
#include <limits.h>
#include <ctype.h>

#include <fcntl.h>
#include "shell.h"
#include "util.h"
#include "input.h"
#include "files.h"
#include <sys/types.h> //pid_t
#include <sys/wait.h>

// Signals
#include <signal.h>

#include "/usr/include/unistd.h"

/**
 * If 'isForground' is -1 prints "job exited with exit code 'exitcode'"
 * else if 'isForeground' is 0 prints "Background job exited with exit code 'exitcode'"
 * else prints "Foreground job exited with exit code 'exitcode'"
 */
#define printExitCode(exitcode, isForeground) \
    printf("%s -%s job exited with exit code %d\033[0m\n\n", "\033[2m", (isForeground == -1 ? " " : \
    (isForeground ? "Foreground" : "Background")), exitcode);
#define CMD_CD ("cd")
#define CMD_EXIT ("exit")

const int CMD_NB = 2;
const char* CMDS[] = {CMD_CD, CMD_EXIT};

struct Shell {
    /** Contains the cwd (of Size PATH_MAX).
     * Copy of current working directory as a field, to not having to refetch it everytime since
     * 'getcwd()' copies the actual each time it is called */
    char* crt_path;
    // Pid of current process launched as foreground job
    pid_t foreground_job;
    // Pid of current process launched as background job
    pid_t background_job;
    // Number of current non-waited/terminated child
    uint child_number;
};


const char* pwd(Shell* sh) { return sh->crt_path; }
pid_t sh_BJ(Shell* sh) { return sh->background_job; }
pid_t sh_FJ(Shell* sh) { return sh->foreground_job; }
/** Getter for 'child_number' field
 * @return Number of current non-waited/terminated child */
uint child(Shell* sh) { return sh->child_number; }

/**
 * "Private" setter for 'crt_path' field
 * @param sh Shell instance
 * @param newPath new path
 */
void set_crt_path(Shell* sh, const char* newPath) {
    sh->crt_path = newPath;
    //maybe handle dynamically here memory allocated to crt_path if it doesn't cause too much overhead to call realloc that many times
}

/**
 * "Private" setter for 'background_job' field
 * @param sh Shell instance
 * @param background_job  pid of background job
 */
void set_BJ(Shell* sh, pid_t background_job) { sh->background_job = background_job; }

/**
 * "Private" setter for 'foreground_job' field
 * @param sh Shell instance
 * @param foreground_job  pid of foreground job
 */
void set_FJ(Shell* sh, pid_t foreground_job) { sh->foreground_job = foreground_job; }

//Global variable referring to the current environment
extern char** environ;

Shell* new_Shell() {
    Shell* sh = tryalc(malloc(sizeof(Shell)));
    sh->crt_path = tryalc(malloc(PATH_MAX));

    if (getcwd(sh->crt_path, PATH_MAX) == NULL) {
        printErr("%s - cannot initialize cwd (%s)\n", sh->crt_path);
        return NULL;
    }
    sh->background_job = 0;
    sh->foreground_job = 0;
    sh->child_number = 0;
    //maybe register a function to wait for children in atexit
    return sh;
}

/**
 * Wait for all children to die, then terminates them with wait()
 */
void terminate_all_children(Shell* sh) {
    // until shell process has child:
    while (child(sh) > 0) {
        int exitStatus;
        /*int decrement = 1;
        if (wait(&exitStatus) == -1) {
            int savedErr = errno;
            if (savedErr != EINTR && savedErr != ECHILD)
                fprintf(stderr, "clean_exit: %s: cannot kill child.\n", strerror(savedErr));
            else if (savedErr == EINTR) decrement = 0; // for that error just retry
        }*/
        wait_s(&exitStatus);
        printExitCode(exitStatus, -1);
        sh->child_number -= 1;
    }
}

/**
 * Perform different cleanup actions before exiting.
 * E.g. kill/wait for all childs still alive / zombified to avoid orphans,
 * free fields 'sh'... Then exits
 *
 * @param sh ptr to Shell instance
 * @param exitCode exit code to exit with
 */
void clean_exit(Shell* sh, int exitCode) {
    terminate_all_children(sh);
    if (sh != NULL) sh_free(sh);
    exit(exitCode);
}

/**
 * Changes the current working directory to the one specified by the path argument
 * @param sh Shell*, ptr to instance of Shell
 * @param path The path (relative or absolute) to the directory to change to.
 * @return Exit code. 0 for success, -1 for error.
 */
int cd(Shell* sh, const char* path) {
    char* resolvedPath;
    if (!path) resolvedPath = getenv("HOME");
    else {
        int isAbsolute = (*path == '/');
        if (isAbsolute) resolvedPath = absPath(path);
        else {
            const char* tmp = concat_path(pwd(sh), path);
            resolvedPath = absPath(tmp);
            free(tmp);
        }
        if (resolvedPath == NULL) printRErr("cd: %s : %s", path);
    }

    if (chdir(resolvedPath) < 0) {
        printRErr("cd : %s - %s\n", path);
    } else
        set_crt_path(sh, resolvedPath);

    // if successfully changed path:
    return EXIT_SUCCESS;
}


/**
 * Exit shell with given exitcode by calling 'clean_exit()' once 'arg' was parsed into an int
 * @param arg exitcode as string
 */
void exit_shell(Shell* sh, const char* arg) {
    int exit_code;
    if (!arg) exit_code = 0;
    else {
        int err = strToInt(arg, 10, &exit_code);
        // if conversion failed, only change exit_code if there was no error before (i.e. exit_code == "EXIT_SUCCESS")
        if (err < 0 && exit_code == EXIT_SUCCESS) exit_code = EXIT_FAILURE;
    }
    printf("Exit with exit code %d\n\n", exit_code);

    clean_exit(sh, exit_code);
}

/** Utility function if we ever need to do something to environ (e.g. pretreat it...) before returing it*/
char** getEnvp() {
    return environ;
}


//void because exit on error
void redirectIO() {
    const char* redirection = "/dev/null";
    //close stdin, stdout stderr
    close(2);
    int fd = open(redirection, O_RDWR);
    for (int i = 0; i < 3; i++) {
        dup2(fd, i);
    }

    /*for (int i = 0; i < 3; i++)
        dup2()

        if (close(i) < 0) hdlCloseErr("stdin/out/err", 1);*/

    // reopens 3times /dev/null, so that the 3 first file descriptors points to it.
    // (3rd first fd's are always std in/out/err)
    /*for (int i = 0; i < 3; i++) {
        if (open(redirection, (i == 0 ? O_RDONLY : O_RDWR) | O_EXCL))
            hdlOpenErr(redirection, 1);
    }*/
}


/**
 * Call execvpe, handle errors and print "Foreground job exited with exit code <errorcode extracted when handling error>"
 */
int exec(Shell* sh, const char* filename, char* const argv[], int isForeground) {
    errno = 0;
    int exitcode = 0;
    //TODO: replace execvpe by another exec* because it is a GNU extension (not POSIX)
    // "The execvpe() function is a GNU extension." source: man excevpe

    if (execvpe(filename, argv, getEnvp()) < 0) {
        // sets
        if (isForeground) sh->foreground_job = -1;
        else sh->background_job = -1;
        exitcode = errno;
        fprintf(stderr, "%s: \"%s\" \n", exitcode == ENOENT ? "command not found" : strerror(exitcode), filename);
    }
    return -1;
    // if exec command returns then there have been an error somewhere
}


//TODO: document this

int executeJob(Shell* sh, const char* cmd_name, char* const argv[], int isForeground) {
    if (cmd_name == NULL || strlen(cmd_name) <= 0) return -1;
    pid_t t_pid = fork();
    int child_exitcode = EXIT_FAILURE;

    if (t_pid < 0) {
        printErr("executeJob: %s, %s - Cannot Fork.\n", argv[0]);
        //printf("Foreground job exited\n\n");
        //printExitCode(child_exitcode, isForeground);
        //- No job exited because no job were created in the first place
        return -1;
    }

    //* In parent
    if (t_pid > 0) {
        sh->child_number += 1;

        if (isForeground) {
            set_FJ(sh, t_pid);
            //- Only wait for foreground jobs
            if (wait_s(&child_exitcode) == EXIT_SUCCESS) {
                sh->child_number -= 1;
                printExitCode(child_exitcode, isForeground);
                return EXIT_SUCCESS;
            } else return -1;

        } else {
            set_BJ(sh, t_pid);
            printf("[1] \t[%d] - %s\n", sh_BJ(sh), cmd_name);
            return EXIT_SUCCESS;
        }
    }
    if (t_pid == 0) {
        //* In child
        if (!isForeground) redirectIO();
        if (exec(sh, cmd_name, argv, isForeground) < 0)
            exit(EXIT_FAILURE);
    }
    return -1;
    //- If we're in parent we've returned with the return above and if we're child we've exited
}


int sh_getAndResolveCmd(Shell* sh) {
    //TODO: Check for & => and make background job
    int argc = 0, isForeground = 1;
    const char** argv = readParseIn(&argc, &isForeground);
    if (argc <= 0 || argv == NULL) {
        printRErr("%s: Could not parse user input - read %d argument.\n", argc); //returns -1
        puts(" as");
    }
    const char* cmd_name = argv[0];
    switch (strswitch(cmd_name, CMDS, CMD_NB)) {

        case 0:
            // CMDS[0] is "cd". => cd to 2nd argument in argv ignoring the rest.
            if (cd(sh, argc <= 1 ? NULL : argv[1]) < 0) return -1;
            printf("\n");
            break;
        case 1:
            // CMDS[1] is "exit"
            // if no exit code 2nd arg is blank => exit with default code
            exit_shell(sh, (argc <= 1 || *argv[1] == '\n') ? NULL : argv[1]);

        default: {
            //TODO: handle background jobs
            if (executeJob(sh, cmd_name, argv, isForeground) < 0) return -1;
        }
    }

    return EXIT_SUCCESS;
}


void sh_free(Shell* sh) {
    free(sh->crt_path);
    free(sh);
    // 'crt_path' and 'sh' were the only "malloc'ed" variables
}



// -----------------  Signal handling ---------------------

struct sigaction sa;
printf("Pid: %d\n", getpid());

sa.sa_sigaction = manage_signals;

sigemptyset(&sa.sa_mask);

sigaddset(&sa.sa_mask, SIGTERM);
sigaddset(&sa.sa_mask, SIGQUIT);
sigaddset(&sa.sa_mask, SIGINT);
sigaddset(&sa.sa_mask, SIGHUP);
sigaddset(&sa.sa_mask, SIGCHLD);

sa.sa_flags = 0;

if (sigaction(SIGUSR1, &sa, NULL) == -1) {
    perror("setting up SIGUSR1");
    return 1;
}

if (sigaction(SIGTERM, &sa, NULL) == -1) {
    perror("setting up SIGTERM");
    return 1;
}
sa.sa_handler = usr2_exit;
sigemptyset(&sa.sa_mask);
if (sigaction(SIGUSR2, &sa, NULL) == -1) {
    perror("setting up SIGUSR1");
    return 1;
}


void manage_signals(int sig) {
    switch (sig) {

    case SIGTERM:
        // something here
        break;

    case SIGQUIT:
        // something here
        break;

    case SIGINT:
        // something here
        break;
    
    case SIGHUP:
        // something here
        break;
    
    case SIGCHLD:
        // something here
        break;

    default:
        break;

    }
}

// ---------------------------------------------------------












// ================ Printing functions ====================

/* void printPid(int in) {
    char* msg = in ? "PID=%d> " : "[PID=%d] ";
        printf(msg, getpid());
} */

void listEnv() {
    for (char** current = getEnvp(); *current; current++) puts(*current);
}

//! 0:default,  1:red, 2:Green,  3:Blue, 4:Purple, 5:yellow,  6:cyan,  7:grey
//const char* colors[] = {"\033[0m", "\033[0;31m", "\033[0;32m", "\033[0;34m", "\033[0;35m", "\033[0;33m", "\033[0;36m",
  //                      "\033[2m"};
extern char* colors[];

/**
 * 0:default,  1:red, 2:Green,  3:Blue, 4:Purple, 5:yellow,  6:cyan
 */
void setOutColor(int color) {
    printf("%s", colors[color]);
}

void resetCol() {
    setOutColor(0);
}

void sh_prettyPrintPath(Shell* sh) {
    printf("%s( %s", colors[1], colors[6]);
    printf("%s", pwd(sh));
    printf("%s )\n", colors[1]);
    //printf("|_ %s$ ", colors[5]);
    resetCol();
}
