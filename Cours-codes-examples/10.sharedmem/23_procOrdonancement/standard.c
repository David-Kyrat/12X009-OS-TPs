#include <sys/resource.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include"../common/common.h"

#define LOOPS 100

void work(pid_t pid, const char* name)
{
    int i;
    for(i=0;i<LOOPS;i++)
        printf("Le processus %s (PID %d) travaille pour la %deme fois\n",name, pid, i);
}

int main(int argc, char* argv[])
{
    int prio;
    pid_t pid;
    
    if(argc != 2)
        OnError("Un argument (uniquement) doit etre passé au programme");
    
    prio = atoi(argv[1]);    
    
    pid = fork();
    if(pid == 0) /* fils */
    {
        setpriority(PRIO_PROCESS, 0, prio);
        work(getpid(), "fils");
    }
    else if(pid > 0) /* père */
    {
        work(getpid(), "pere");
    }
    else
        OnError("fork:");
    
    return 0;
    
}
