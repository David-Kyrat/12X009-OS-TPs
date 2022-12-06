#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

void manage_signals(int sig) {
    switch (sig)
    {
    case SIGUSR1:
        write(STDOUT_FILENO, "SIGUSR1\n", 8);
        break;
    case SIGTERM:
        write(STDOUT_FILENO, "SIGTERM\n", 8);
        break;
    default:
        break;
    }
}

void usr2_exit(int sig) {
    write(STDOUT_FILENO, "Received SIGUSR2, process exiting\n", 34);
    exit(0);
}

int main() {

    struct sigaction sa;

    printf("Pid: %d\n", getpid());

    sa.sa_handler = manage_signals;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGUSR1);
    sigaddset(&sa.sa_mask, SIGTERM);
    sa.sa_flags = 0;
    if (sigaction(SIGUSR1, &sa, NULL) == -1) { // "enables" using the handles/function given the struct in sa
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

    while(1)
        pause();  // Sleep and wait for a signal

    return 0;
}
