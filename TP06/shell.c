#include <asm-generic/errno-base.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>  // Signals
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>  //pid_t
#include <sys/wait.h>
#include <unistd.h>

#include "shell.h"
#include "files.h"
#include "input.h"
#include "util.h"


/**
 * If 'isForground' is -1 prints "job exited with exit code 'exitcode'"
 * else if 'isForeground' is 0 prints "Background job exited with exit code 'exitcode'"
 * else prints "Foreground job exited with exit code 'exitcode'"
 */
#define printExitCode(exitcode, isForeground) \
    printf("%s -%s job exited with exit code %d\033[0m\n\n", "\033[2m", (isForeground == -1 ? " " : (isForeground ? "Foreground" : "Background")), exitcode);
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
    int child_number;

    int old_fj_argc;
    char*** old_fj; //pointer to argv of last foreground job
    int old_bj_argc;
    char*** old_bj; //pointer to argv of last background job
};

pid_t sh_FJ(Shell* sh) { return sh->foreground_job;}
pid_t sh_BJ(Shell* sh) { return sh->background_job; }

char** sh_oldFJ(Shell* sh) {return *(sh->old_fj);}

char** sh_oldBJ(Shell* sh) {return *(sh->old_bj);}

char** sh_oldJob(Shell* sh, int isForeground) { return isForeground ? sh_oldFJ(sh) : sh_oldBJ(sh); }

int sh_oldBJ_argc(Shell* sh) {return sh->old_bj_argc;}

int sh_oldFJ_argc(Shell* sh) {return sh->old_fj_argc;}

int sh_old_argc(Shell* sh, int isForeground) { return isForeground ? sh_oldFJ_argc(sh) : sh_oldBJ_argc(sh);}


const char* pwd(Shell* sh) { return sh->crt_path; }
/** Getter for 'child_number' field
 * @return Number of current non-waited/terminated child */
int child(Shell* sh) { return sh->child_number; }

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

/**
 * If the number of child processes of the shell is > 0 => decrease it by -1
 * @param sh the shell
 * @return exit code 1 if it had > 0 children -1 otherwise
 */
int decrease_childNb(Shell* sh) {
    if (sh->child_number > 0) {
        sh->child_number -= 1;
        return EXIT_SUCCESS;
    }
    return -1;
}

/**
 * Increases the number of child processes by one, and returns an error if the number of child
 * processes is greater than two
 * @param sh the shell
 * @return exit code 1 if it had less than 2 children -1 otherwise
 */
int increase_childNb(Shell* sh) {
    sh->child_number += 1;
    return sh->child_number <= 2 ? EXIT_SUCCESS : -1;
}


void update_old_job(Shell* sh, int isForeground, char*** new_oldArgv, int old_argc) {
    if (isForeground) {
        free(sh_oldFJ(sh)); // free memory containing the argument of second to last job
                            // Does not free the pointer free the char** pointed to by sh->old_fj
        *(sh->old_fj) = *new_oldArgv;
        sh->old_fj_argc = old_argc;
    } else {
        free(sh_oldBJ(sh));
        *(sh->old_bj) = *new_oldArgv;
        sh->old_bj_argc = old_argc;
    }
}

void update_old_fj(Shell* sh, char*** new_oldArgv, int old_fj_argc) { update_old_job(sh, 1, new_oldArgv, old_fj_argc);  }

void update_old_bj(Shell* sh, char*** new_oldArgv, int old_bj_argc) { update_old_job(sh, 0, new_oldArgv, old_bj_argc);  }


//Global variable referring to the current environment
extern char** environ;
void hdl_sigint(Shell* sh);
void hdl_sigchild(Shell*, pid_t);
void hdl_sigup(Shell* sh);


Shell* new_Shell() {
    Shell* sh = tryalc(malloc(sizeof(Shell)));
    sh->crt_path = tryalc(malloc(PATH_MAX));

    if (getcwd(sh->crt_path, PATH_MAX) == NULL) {
        printErr("%s - cannot initialize cwd (%s)\n", sh->crt_path);
        return NULL;
    }
    sh->background_job = -2;
    sh->foreground_job = -2;
    sh->child_number = 0;
    sh->old_bj = malloc(sizeof(char**));
    sh->old_fj = malloc(sizeof(char**));
    sh->old_bj_argc = 0;
    sh->old_fj_argc = 0;

    return sh;
}


/**
 * Wait for all children to die, then terminates them with wait()
 */
void terminate_all_children(Shell* sh) {
    // until shell process has children processes:
    
    if (child(sh) <= 0) printf("\nNo child to terminate.\n");
    while (child(sh) > 0) {
        int exitStatus;
        fprintf(stderr, "Nb of child waiting to be terminated: %d\n", child(sh));
        int code = wait_s(&exitStatus);
        if (code == 2) {
            // there were no child to wait for => sets number of child to 0 and returns
            sh->child_number = 0;
            return;
        }
        else if (code >= 0) {
            decrease_childNb(sh);
            printExitCode(exitStatus, -1);
        }
    }
}

/**
 * Perform different cleanup actions before exiting.
 * E.g. kill/wait for all childs still alive / zombified to avoid orphans,
 * free fields 'sh'... Then exits
 *
 * @param sh ptr to Shell instance
 * @param exitCode exit code to exit with
 * @param forceExit 0 to wait for each job, 1 to kill foreground job, 2 to kill background job, 3 to kill both
 */
void clean_exit(Shell* sh, int exitCode, int forceExit) {
    write(2, "\n", 2);
    int signal = SIGKILL;
    if (forceExit - 2 >= 0) {
        // 2 or 3
        if (sh_BJ(sh) != -2) {
            fprintf(stderr, "Force killing background job.\n");
            kill(sh_BJ(sh), signal);
            set_BJ(sh, -2);
        }            
    } 
    if (forceExit % 2 == 1) {
        // 1 or 3
        if (sh_FJ(sh) != -2){
            fprintf(stderr, "Force killing foreground job.\n");
            kill(sh_FJ(sh), signal);
            set_FJ(sh, -2);
        }
    } // child process will be terminated by the function below

    terminate_all_children(sh);
    sh_free(sh);
    fprintf(stderr, "Exiting with exit code %d.\n", exitCode);
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
    if (!path)
        resolvedPath = getenv("HOME");
    else {
        int isAbsolute = (*path == '/');
        if (isAbsolute)
            resolvedPath = absPath(path);
        else {
            const char* tmp = concat_path(pwd(sh), path);
            resolvedPath = absPath(tmp);
            free(tmp);
        }
        if (resolvedPath == NULL) printRErr("cd: %s : %s\n", path);
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
    if (!arg)
        exit_code = 0;
    else {
        int err = strToInt(arg, 10, &exit_code);
        // if conversion failed, only change exit_code if there was no error before (i.e. exit_code == "EXIT_SUCCESS")
        if (err < 0 && exit_code == EXIT_SUCCESS) exit_code = EXIT_FAILURE;
    }
    printf("Exit with exit code %d\n\n", exit_code);

    clean_exit(sh, exit_code, 3);
}

/**
 * Redirect stdin to /dev/null
 * void because exit on error 
 */
void redirectIO() {
    const char* redirection = "/dev/null";
    //close stdin, stdout
    //- do not close stderr/out
    if (close(0) < 0) hdlCloseErr("stdin/out", 1);
    //reopens /dev/null, so that the first file descriptor points to it.
    // (3 first fd's are always std in/out/err)
    if (open(redirection, O_RDONLY | O_EXCL) < 0) hdlOpenErr(redirection, 1);
}


/**
 * Call execvpe, handle errors and print "Foreground job exited with exit code <errorcode extracted when handling error>"
 */
int exec(Shell* sh, const char* filename, char* const argv[], int isForeground) {
    errno = 0;
    int exitcode = 0;

    if (execvp(filename, argv) < 0) {
        if (isForeground)
            sh->foreground_job = -2;
        else
            sh->background_job = -2;
        exitcode = errno;
        fprintf(stderr, "%s: \"%s\" \n", exitcode == ENOENT ? "command not found" : strerror(exitcode), filename);
    }
    return -1;
    // if exec command returns then there have been an error somewhere
}

//TODO: document this

int executeJob(Shell* sh, const char* cmd_name, char* const argv[], int isForeground) {
    if (cmd_name == NULL || strlen(cmd_name) <= 0) return -1;
    
    if (!isForeground && sh_BJ(sh) != -2) {
        // If there is currently already a background_job running
        errno = EBUSY;
        printRErr("executeJob: %s, background job (%d) is still running. Please wait for its completion or launch it as foreground job.\n", sh_BJ(sh));
    } //returns -1

    pid_t t_pid = fork();
    
    int child_exitcode = EXIT_FAILURE;

    if (t_pid < 0)
        printRErr("executeJob: %s, %s - Cannot Fork.\n", argv[0]); //returns -1

    if (t_pid > 0) {
        //* In parent
        increase_childNb(sh);

        if (isForeground) {
            set_FJ(sh, t_pid);
            //- Only wait for foreground jobs
            int code = wait_s(&child_exitcode);
            if (sh_FJ(sh) == -2) return EXIT_SUCCESS; // if foreground job was killed by a signal just return

            if (code == 2) sh->child_number = 0;
            if (code >= 0) {
                decrease_childNb(sh);
                set_FJ(sh, -2);
                printExitCode(child_exitcode, 1);
                
                return EXIT_SUCCESS;
            } else return -1;

        } else {
            errno = 0; // useful to check actual error when managing signals
            set_BJ(sh, t_pid);
            printf("[1] \t[%d] - %s\n\n", sh_BJ(sh), cmd_name); 
            return EXIT_SUCCESS;
        }
    } 

    if (t_pid == 0) {
        //* In child
        errno = 0;
        if (!isForeground) redirectIO();
        if (exec(sh, cmd_name, argv, isForeground) < 0)
            exit(EXIT_FAILURE);
    }
    return -1;
    //- If we're in parent we've returned with the return above and if we're child we've exited
}


int sh_getAndResolveCmd(Shell* sh) {
    sh_prettyPrintPath(sh);

    int argc = 0, isForeground = 1;
    const char** argv = readParseIn(&argc, &isForeground);
    if (argc <= 0 || argv == NULL) {
        printRErr("%s: Could not parse user input - read %d argument.\n", argc);  //returns -1
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
            int errcode = executeJob(sh, cmd_name, argv, isForeground);
            update_old_job(sh, isForeground, &argv, argc);
            if (errcode < 0) return -1;
        }

    }

    return EXIT_SUCCESS;
}

void sh_free(Shell* sh) {
    free(sh->crt_path);
    free(sh);
        // 'crt_path' and 'sh' were the only "malloc'ed" variables
}



/*
 * =============================================================
 * -------------------  Signal handling ------------------------
 * =============================================================
 */

/** List of signal to handle.*/
const int SIG_TO_HDL[] = {SIGUSR1, SIGINT, SIGHUP, SIGCHLD}; 
//* When usr press CTRL+D => EOF is sent on stdin, once that happen, we send an SIGUSR1 to ourselves
const int SIG_TO_IGNORE[] = {SIGQUIT, SIGTERM};
const int SIG_NB = 4, IGNORE_NB = 2;  // Number of signal to handle

void manage_signals(int sig, siginfo_t* info, Shell* sh) {
    switch (sig) {
        case SIGUSR1:
            // CTRL+D received
            // cleanup before exiting => avoiding zombies and orphans
            clean_exit(sh, 0, 3); //force exiting background and foreground job 
            break;

        case SIGINT:
            write(1, "\n", 2); //Write newline on stdout to "remove" the "^C" that gets generated from current input "line"
            // killing foreground job of 'sh'
            hdl_sigint(sh);
            // cleaning prompt
            display_prompt();
            break;

        case SIGHUP:
            hdl_sigup(sh);
            break;

        case SIGCHLD: 
            hdl_sigchild(sh, info->si_pid);
        break;

        default:
            break;
    }
}


void hdl_sigint(Shell* sh) {
    pid_t fj = sh_FJ(sh);
    if (fj == -2 || fj == 0) return; //if no foreground job do nothing
    if (kill(fj, SIGINT) < 0) {
        printErr("%s: cannot kill foreground job (pid: %d)\n", fj);
        return;
    }
}

void hdl_sigup(Shell* sh) {
    kill(sh_FJ(sh), SIGHUP);
    kill(sh_BJ(sh), SIGHUP);
    set_BJ(sh, -2);
    set_FJ(sh, -2);
    clean_exit(sh, 0, 3);
}

void hdl_sigchild(Shell* sh, pid_t dying_child_pid) {
    int s = errno;

    if (dying_child_pid == sh->background_job) {
        //if (s != 0) fprintf(stderr, "hdl_sigchild, errno: %s\n", strerror(s));
        int exitStatus, code;
        code = waitpid_s(dying_child_pid, &exitStatus);
        if (code == 2) {
            sh->child_number = 0;
        } else if (code >= 0) {
            decrease_childNb(sh);
            printExitCode(exitStatus, 0);
        }
        set_BJ(sh, -2);

        if (s == EINTR) {
            // if previous job was stopped by a signal => relaunch it
            const char** argv = sh_oldBJ(sh);
            //set_BJ(sh, -2); // setting bj pid to -2 so that the relaunching of bj doesnt get denied (we can only have 1 background_job)
            const char* msg = "\ninterrupted by signal, restarting background job...\n";
            write(2, msg, strlen(msg)+1);
            executeJob(sh, argv[0], argv, 0);
        }
        display_prompt();
    }   
}


/**
 * Initialize signal handlers.
 * 
 * @param sh Shell
 * @param sig_hdler Signal handler (which do not take a Shell instance as argument) to use (see 'manage_signals' for example)
 * @return int -1 if an error occured, 0 otherwise.
 */
int initSigHandlers(Shell* sh, void (*sig_hdler)(int, siginfo_t* info, void* ucontext)) {
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO | SA_RESTART;

    printf("Pid: %d\n", getpid());
    sa.sa_sigaction = sig_hdler; 

    sigemptyset(&sa.sa_mask);
    for (int i = 0; i < SIG_NB; i++)
        sigaddset(&sa.sa_mask, SIG_TO_HDL[i]);

    //
    //* Handle Signals to handle
    //
    for (int i = 0; i < SIG_NB; i++) {
        if (sigaction(SIG_TO_HDL[i], &sa, NULL) == -1) {
            const char msg[15];
            sprintf(msg, "SIG_TO_HDL[%d]\n", i);
            hdlSigHdlErr(msg, 1);
        }
    }
    //
    //* Ignore Signals to ignore
    //
    struct sigaction sa_ign;  //use same sa struct does not work
    
    sigemptyset(&sa_ign.sa_mask);
    for (int i = 0; i < SIG_NB; i++) sigaddset(&sa_ign.sa_mask, SIG_TO_IGNORE[i]);

    
    sa_ign.sa_handler = SIG_IGN;
    sa_ign.sa_flags = SA_RESTART;
    for (int i = 0; i < IGNORE_NB; i++) {
        if (sigaction(SIG_TO_IGNORE[i], &sa_ign, NULL) == -1) {
            const char msg[20];
            sprintf(msg, "SIG_TO_IGNORE[%d]\n", i);
            hdlSigHdlErr(msg, 0);
        }
    }

    return EXIT_SUCCESS;
}
// ------------------- END SIGNALS -----------------



/**
 * ========================================================
 * ---------------- Printing functions --------------------
 * ========================================================
 */


void listEnv() {
    for (char** current = environ; *current; current++) puts(*current);
}

//! 0:default,  1:red, 2:Green,  3:Blue, 4:Purple, 5:yellow,  6:cyan,  7:grey
extern char* colors[];

/**
 * 0:default,  1:red, 2:Green,  3:Blue, 4:Purple, 5:yellow,  6:cyan
 */
void setOutColor(int color) { printf("%s", colors[color]); }

void resetCol() { setOutColor(0); }

void sh_prettyPrintPath(Shell* sh) {
    printf("%s( %s", colors[1], colors[6]);
    printf("%s", pwd(sh));
    printf("%s )\n", colors[1]);
    //printf("|_ %s$ ", colors[5]);
    resetCol();
}

void display_prompt() {
    //sh_prettyPrintPath(sh);
    const char* tmp = getPrompt();
    int len = strlen(tmp);
    write(1, tmp, len+1);
}
