#ifndef SHELL_H
#define SHELL_H

#include <unistd.h>

/**
 * Represents a Shell with a crt_path (pwd) a foreground job and 
 * a background_job. Crt_path default value is where the executable was called
 *
 *
 *  NB: All shell methods/functions starts with sh_ to easily identify them (except 'new_Shell' and 'pwd')
 */
typedef struct Shell Shell;


/**
 * Shell primary constructor. Return a pointer to a new instance
 * of Shell with crt_path initialized at where the executable was called
 * and no foreground_job nor background_job
 * 
 * @return pointer to a new instance of Shell
 */
Shell* new_Shell();

/**
 * Returns the current working directory
 * 
 * @return The current working directory.
 */
const char* pwd(Shell* sh);


/**
 * Call 'readParseIn()' to get the parsed user input =>
 * then resolve the requested command / program to launch & execute them accordingly.
 *
 * @return Exit code. 0 on success, -1 otherwise.
 */
int sh_getAndResolveCmd(Shell* sh);

/**
 * Frees fields of given ptr to Shell
 */
void sh_free(Shell* sh);

// =================== GETTERS & Other =====================

/**
 * Getter for 'background_job' field
 * @param sh pointer to Shell instance
 * @return background_job field for 'sh'
 */
pid_t sh_BJ(Shell* sh);

/**
 * Getter for 'foreground_job' field
 * @param sh pointer to Shell instance
 * @return foreground_job field for 'sh'
 */
pid_t sh_FJ(Shell* sh);

/** TODO:Comment
 * asd
 */
void manage_signals(int sig, siginfo_t* info, Shell* sh);

/** TODO:Comment */
int initSigHandlers(Shell* sh, void (*sig_hdler)(int, siginfo_t* info, void* ucontext));


/**
 * Print the current working directory to console output
 */
void sh_prettyPrintPath(Shell* sh);

/**
 * print '|_ $ ' (without using printf)
 */
void display_prompt();

#endif /* SHELL_H */
