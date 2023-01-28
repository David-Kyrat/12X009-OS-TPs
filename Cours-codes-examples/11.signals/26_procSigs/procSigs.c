#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "../common/common.h"

void displaySigSet(sigset_t set)
{
    if(sigismember(&set, SIGUSR1))
        printf("SIG_USR1 is in set\n");
    if(sigismember(&set, SIGUSR2))
        printf("SIG_USR2 is in set\n");        
}


void handler(int sig)   
{
    sigset_t getsig;
	
    printf("Proc %d as received a user signal %d, it will sleep for 5 seconds...\n", getpid(), sig);
    sleep(5);	

    //Display the signals received while they were masked
    printf("Getting out of handler, checking pending signals first\n");
    if(sigpending(&getsig) == -1)
        OnError("sigpending");
    displaySigSet(getsig);

    return;
}

int main(void)
{
    struct sigaction sa;
    sigset_t setsig;

    //Setup the handler
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
    
    //Display process PID
    printf("My PID: %d\n", getpid());
    
    //Send a demonstration of the USR1 signal
    raise(SIGUSR1);
    
    //First mask the USR1 and USR2 signals
    sigemptyset(&setsig);
    sigaddset(&setsig, SIGUSR1);
    sigaddset(&setsig, SIGUSR2);	
    if(sigprocmask(SIG_BLOCK, &setsig, NULL) == -1)
    	OnError("sigprocmask");
    printf("The process cannot receive user signals for the next 10 seconds...\n");

    //Wait for 10 seconds
    sleep(10);

    //Unmask the USR1 and USR2 signals
    if(sigprocmask(SIG_UNBLOCK, &setsig, NULL) == -1)
	    OnError("sigprocmask (unmask)");

    //Create a mask excluding only SIGUSR1 and SIGUSR2
    sigfillset(&setsig);
    sigdelset(&setsig, SIGUSR1);
    sigdelset(&setsig, SIGUSR2);	
    printf("From now on only USR1 and USR2 signals are received\n");	
    while(1)
        sigsuspend(&setsig);

    return 0;
}


