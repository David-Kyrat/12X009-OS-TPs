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
#include "shell.h"
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
/** 
 * global variable to be able to see it when doing signal handling => won't be modified there though, because the handler mustn't modify global variables
 * sums up the amount of child process for all instances of the struct Shell.
 */
int tot_child_nb;
//TODO: find a way to keep it up to date even after signal managment that may terminate child without modifying this

struct Shell {
    /** Contains the cwd (of Size PATH_MAX).
     * Copy of current working directory as a field, to not having to refetch it everytime since
     * 'getcwd()' copies the actual each time it is called */
    char* crt_path;
    // Pid of current process launched as foreground job
    pid_t foreground_job;
    // Pid of current process launched as background job
    pid_t background_job;
    // Actual number of current non-waited/terminated child process createad by this specific instance of 'Shell'
    int child_number;
};

pid_t sh_FJ(Shell* sh) { return sh->foreground_job;}
pid_t sh_BJ(Shell* sh) { return sh->background_job; }

const char* pwd(Shell* sh) { return sh->crt_path; }
/** Getter for 'child_number' field
 * @return Number of current non-waited/terminated child process created by the given instance of 'Shell'*/
int child(Shell* sh) {  return sh->child_number; }

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
void set_FJ(Shell* sh, pid_t foreground_job) { sh->foreground_job = foreground_job;}


/**
 * If the number of child processes of the shell is > 0 => decrease it by -1
 * @param sh the shell
 * @return exit code 1 if it had > 0 children -1 otherwise
 */
int decrease_childNb(Shell* sh) {
    //int chld_nb = *(sh->child_number);
    if (sh->child_number  > 0) {
        tot_child_nb -= 1;
        sh->child_number -= 1;
        fprintf(stderr, "decreasead child_number, now at %d\n", tot_child_nb);
        return EXIT_SUCCESS;
    }
    fprintf(stderr, "child_number already at 0, now at %d\n", tot_child_nb);
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
    //int chld_nb = *(sh->child_number);
    //fprintf(stderr, "increased child_number, now at %d\n", chld_nb);
}


//Global variable referring to the current environment
extern char** environ;
void hdl_sigint(Shell* sh);
void manage_signals(int sig, siginfo_t* info, Shell* sh);
void (*hdl_store)(int, siginfo_t* info, void* ucontext);

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

    //initSigHandlers(sh);

    return sh;
}

/**
 * Terminates for all children processes (created by the given instance of 'Shell') with wait()
 */
void terminate_all_children(Shell* sh) {
    // until shell process has child:
    /*            if (wait(&exit_status) < 0) {
                printErr("%s: hdl_sigint, FG pid: \n", sh->foreground_job);
            }*/
    

    while (child(sh) > 0) {
        int exitStatus;
        //wait_s(&exitStatus);

        write(2, "waiting", 7);
        fprintf(stderr, " child_nb: %d\n", child(sh));
        wait_s(&exitStatus);
        /*int se = errno;
        fprintf(stderr, "waiting tal: %s\n", strerror(se));
        printExitCode(exitStatus, -1);*/
        decrease_childNb(sh);

        fprintf(stderr, "decresead child_number\n");
    }
}

/**
 * Terminates all children processes created by any instance of 'Shell' (of the process that called this function) with wait()
 * @param modify - whether to modify the actual global variable or not. 
 */
void terminate_all_children_global(int modify) {
    int child_nb_cp = tot_child_nb;
    int exitStatus;
    while (child_nb_cp > 0) {
        wait_s(&exitStatus);
        printExitCode(exitStatus, -1);
        child_nb_cp -= 1;
    }
    if (modify) tot_child_nb = child_nb_cp;

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
    //sh_free(sh);
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
    if (!arg)
        exit_code = 0;
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
    //close stdin, stdout
    //- do not close stderr
    close(STDOUT_FILENO);
    int fd = open(redirection, O_RDWR);
    for (int i = 0; i < 2; i++) dup2(fd, i);

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
        increase_childNb(sh);
        //printf("Child_nb: %d\n", child(sh));
        if (isForeground) {
            set_FJ(sh, t_pid);
            printf("FG: %d\n", sh->foreground_job);
            printf("BG: %d\n", sh->background_job);
            //- Only wait for foreground jobs
            if (wait(&child_exitcode) < 0) {
                int err = errno;
                if (err == ECHILD) decrease_childNb(sh);
                else fprintf(stderr, "%s - pid:%d\n", strerror(err), t_pid);
            }
            else {
                printExitCode(child_exitcode, 1);
                decrease_childNb(sh);
            }
            //if (wait_s(&child_exitcode) == EXIT_SUCCESS) {
            /* decrease_childNb(sh);
                printExitCode(child_exitcode, isForeground); */
            return EXIT_SUCCESS;
            //} else return -1;

        } else {
            set_BJ(sh, t_pid);
            printf("FG: %d\n", sh->foreground_job);
            printf("BG: %d\n", sh->background_job);

            printf("[1] \t[%d] - %s\n", sh_BJ(sh), cmd_name);
            return EXIT_SUCCESS;
        }

        return EXIT_SUCCESS;
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
        printRErr("%s: Could not parse user input - read %d argument.\n", argc);  //returns -1
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
    if (sh != NULL) {
        if (sh->crt_path != NULL) {
            free(sh->crt_path);
            sh->crt_path = NULL;
        }
        free(sh);
        sh = NULL;
        // 'crt_path' and 'sh' were the only "malloc'ed" variables
    }
}



/*
 * =============================================================
 * -------------------  Signal handling ------------------------
 * =============================================================
 */

/** List of signal to handle.*/
const int SIG_TO_HDL[] = {SIGTERM, SIGQUIT, SIGINT, SIGHUP, SIGCHLD};
const int SIG_TO_IGNORE[] = {SIGCHLD, SIGQUIT};
const int SIG_NB = 5, IGNORE_NB = 2;  // Number of signal to handle

//TODO: DIRE AUX ASSITANTS QU'IL YA UNE FAUTE DANS L'ENONCE SIGQUIT EST ENVOYÉ PAR CTRL+D
//TODO: IL NE DOIT DONC PAS ETRE IGNORE

void hdl_sigint(Shell* sh) {
    if (sh_FJ(sh) != -2) {
        if (kill(sh->foreground_job, SIGTERM) < 0) {
            printErr("%s: cannot kill foreground job (pid: %d)\n", sh_FJ(sh));
            return;
        }
        int exit_status;
        if (wait_s(&exit_status) >= 0) {
            decrease_childNb(sh);
            printExitCode(exit_status, 1);
        }
    }
}


void manage_signals(int sig, siginfo_t* info, Shell* sh) {
    switch (sig) {
        case SIGTERM:
            // cleanup before exiting => avoiding zombies and orphans

            // Do nothing when ctrl+d is pressed for some reason, => CTRL+D is not SIGTERM ?
            break;

        case SIGQUIT:
            write(STDERR_FILENO, "quit\n", 6);
            // something here
            break;

        case SIGINT:
            hdl_sigint(sh);
            /*const char* msg = "I will not be stopped, I will not go easy.\n";
            write(STDOUT_FILENO, msg, strlen(msg)+1);*/

            //How to get the pid of the foreground job ???
            //=> use sig_action and pass Shell* sh as void pointedr$
            //! => So initSigHandlers has to be called in constructor i.e. in "new_Shell()"

            // killing foreground job of sh
            break;

        case SIGHUP:
            write(STDERR_FILENO, "hup\n", 5);
            // something here
            break;

        case SIGCHLD: {
            // something here
            int exitStatus;
            if (wait(&exitStatus) == -1) {
                int savedErr = errno;
                if (savedErr != EINTR && savedErr != ECHILD) {
                    fprintf(stderr, "%s: cannot kill child.\n", strerror(savedErr));
                    return;
                } else if (savedErr == EINTR) {
                    const char msg[] = "interrupted retrying\n";
                    write(2, msg, strlen(msg) + 1);
                    return;
                } else {
                    const char msg[] = "no child \n";
                    write(2, msg, strlen(msg) + 1);
                }
            }
            decrease_childNb(sh);
            printExitCode(exitStatus, -1);
            write(STDERR_FILENO, "child\n", 7);
        } break;

        default:

            write(STDERR_FILENO, "defa\n", 6);
            break;
    }
}


//TODO: COMMENT THIS

int initSigHandlers(Shell* sh) {
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO | SA_RESTART;

    printf("Pid: %d\n", getpid());

    //sa.sa_sigaction = manage_signals;

    // wrapper for manage signals that has access to a shell instance in context  (we can't pass it as argument
    // so this is the only way of accessing an instance of 'Shell' which is not a global variable)
    void manage_signals_wrapper(int signum, siginfo_t* info, void* ucontext) {
        manage_signals(signum, info, sh);
    }
    /* void** test = malloc(sizeof(void*));
    *test = manage_signals_wrapper; */

    hdl_store = &manage_signals_wrapper;
    sa.sa_sigaction = hdl_store; 

    //sa.sa_sigaction(1, NULL, NULL); // calling here hdl_store seems to work
    //? note: hdl_store == *test, and (*hdl_store) gives {void (int, siginfo_t *, void *)} 0x7fffffffd9b8 where 0x7f... is the same addr as *test and hdl_store
    // using void* ptr to store manage does not work
    

    if (2 == 1) printf("2\n");

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
    struct sigaction sa_ign;  //use same sa make everything bug
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

/* void printPid(int in) {
    char* msg = in ? "PID=%d> " : "[PID=%d] ";
        printf(msg, getpid());
} */

void listEnv() {
    for (char** current = getEnvp(); *current; current++) puts(*current);
}

//! 0:default,  1:red, 2:Green,  3:Blue, 4:Purple, 5:yellow,  6:cyan,  7:grey
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
}

