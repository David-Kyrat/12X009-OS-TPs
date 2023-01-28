#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#define TIME_CPU_USAGE 4
#define MAX_NB_PROC 1000;

static int nbHandlerCalled = 0;

void endOnError(const char* str)
{
    perror(str);
    exit(errno);
}

struct rlimit printLimit(const char* name, int ressource)
{
    struct rlimit limit;    
    if(getrlimit(ressource, &limit) != 0)
        endOnError("getrlimit");

    printf("-- %s -- (%ld = infinity)\n", name, RLIM_INFINITY);
    printf("Limit courante: %ld\n", limit.rlim_cur);
    printf("Limit maximum: %ld\n", limit.rlim_max);
    return limit;
}

void simpleHandler(int signal)
{
    printf("CPU time exceeded !\n");
    nbHandlerCalled++;
}

int main(int argc, char* argv[])
{

    struct rlimit dataLimit, cpuLimit, forkLimit;
    struct sigaction handleSig;
    pid_t *procId; //table of procId
    long int i, count;

    ////////////////
    //Display all and get CPU max time and data segment size
    cpuLimit = printLimit("CPU time", RLIMIT_CPU);
    dataLimit = printLimit("Taille segment de donnée", RLIMIT_DATA);
    //printLimit("Nombre de lock", RLIMIT_LOCKS);
    printLimit("Max mémoire lock", RLIMIT_MEMLOCK);
    forkLimit = printLimit("Nombre de fork", RLIMIT_NPROC);
    printLimit("Nombre de page en mémoire physique", RLIMIT_RSS);
    printLimit("Taille de la pile", RLIMIT_STACK);

    ///////////////
    //Test the fork limit

    //Decrease the fork limit (out of memory before out nb proc limit is exceeded)
    forkLimit.rlim_cur = MAX_NB_PROC;
    if(setrlimit(RLIMIT_NPROC, &forkLimit) != 0)
        endOnError("setrlimit:nbproc");
    printLimit("New limit for number of processes", RLIMIT_NPROC);

    //Create a table of more process id than the limit of proc
    if((procId = (pid_t*) calloc(forkLimit.rlim_cur+10, sizeof(pid_t))) == NULL)
        endOnError("calloc");

    //Create new processes
    for(i=0;i<forkLimit.rlim_cur+10;i++)
    {
        procId[i] = fork();
        if(procId[i] == 0)
            exit(0); //all child remain as zombies... (not much ressources taken but still existing)    
        else if(procId[i] < 0)
        {
            perror("fork");
            printf("Le %ldeme processus n'as PAS été créé\n", i);       
            break;
        }
    }    

    ///////////////
    //Test the CPU limit
    //Set the CPU limit in seconds
    cpuLimit.rlim_cur = TIME_CPU_USAGE;
    if(setrlimit(RLIMIT_CPU, &cpuLimit) != 0)
        endOnError("setrlimit:cpu");

    //Set a new handler for the CPU time signal
    handleSig.sa_handler = simpleHandler;
    sigemptyset(&handleSig.sa_mask);
    handleSig.sa_flags = 0;    
    if(sigaction(SIGXCPU, &handleSig, NULL) != 0)
        endOnError("sigaction");        

    //Make the process work up to the moment where CPU time is exceeded 5 times
    printf("Process is computing...\n");
    while(nbHandlerCalled < 5);

    return 0;
}

