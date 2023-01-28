#include <sys/resource.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sched.h>
#include <time.h>
#include"../common/common.h"

#define LOOPS 1000000000
#define NBFILS 10
#define PRIO_PERE 2
#define PRIO_FILS 3

void work(pid_t pid, const char* name)
{
    int i;
    for(i=0;i<LOOPS;i++);
        //printf("Le processus %s (PID %d) travaille pour la %deme fois\n",name, pid, i);
}

int main(void)
{
    int i;
    pid_t pid;
    struct sched_param schedParamWrite,schedParamRead;
    struct timespec start, end;
    
    //Définition de la priorité du processus père
    schedParamWrite.sched_priority = PRIO_PERE;
    if(sched_setscheduler(0, SCHED_FIFO, &schedParamWrite) != 0)
        OnError("sched_setscheduler pere");
    
    //On crée N fils à la même priorité
    schedParamWrite.sched_priority = PRIO_FILS;
    for(i=0;i<NBFILS;i++)
    {
        // Création d'un fils
        if((pid = fork()) < 0)
                OnError("fork:");
        if(pid == 0) /* fils */
        {
            //Gestion du nom du fils (en cas d'affichage)
            char name[10];
            sprintf(name, "fils%d",i);

            //Le fils dit qu'il commence à travailler
            if(sched_getparam(0, &schedParamRead) != 0)
                OnError("sched_getparam fils");
            printf("Fils %d, je commence a travailler en prio %d\n", i, schedParamRead.sched_priority);

            //Le fils commence sont travail et ensuite affiche la durée du travail
            clock_gettime(CLOCK_REALTIME, &start);
            work(getpid(), name);
            clock_gettime(CLOCK_REALTIME, &end);
            printf("Fils %d, start = %d, stop = %d\n", i, start.tv_sec, end.tv_sec);
            return 0;
        }

        // Ici c'est le père qui lit et gère la priorité des fils        
        
	if(sched_getparam(0, &schedParamRead) != 0)
        	OnError("sched_getparam père");
 	printf("Pere, je viens de creer un fils en priorite %d\n", schedParamRead.sched_priority);
	if(sched_setscheduler(pid, SCHED_FIFO, &schedParamWrite) != 0)
            OnError("sched_setscheduler fils");
    }

    // Le père dit qu'il commence son travail
    if(sched_getparam(0, &schedParamRead) != 0)
        OnError("sched_getparam père");
    printf("Pere, je commence a travailler en prio %d\n", schedParamRead.sched_priority);
    
    //Le père commence sont travail et ensuite affiche la durée du travail
    clock_gettime(CLOCK_REALTIME, &start);
    work(getpid(), "Pere");
    clock_gettime(CLOCK_REALTIME, &end);
    printf("Père, start = %d, stop = %d\n", start.tv_sec, end.tv_sec);    
    
    // On attends les enfants pour finir proprement
    for(i=0;i<NBFILS;i++)
        wait(NULL);
    
    return 0;    
}
