#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "../common/common.h"
int main(void)
{
	pid_t pid;  //Pour sauver le retour de la fonction fork
    
	//Creation d'un nouveau processus
	pid = fork();

    //Depedant du retour soit on est dans le père, dans le fils ou retour erreur
	if(pid == 0) // fils
	{
		printf("Je suis %d fils de %d ET j'attends 20 secondes\n", getpid(), getppid());
		sleep(20);
		printf("Je suis %d fils de %d ET je meurt\n", getpid(), getppid());
	}
	else if(pid > 0) //père
	{
		printf("Je suis %d père de %d ET j'attend 10 seconds\n", getpid(), pid);
		sleep(10);
		printf("Je suis %d père de %d ET je meurt\n", getpid(), pid);
	}
	else //erreur
		OnError("Could not fork\n");
	

	return 0;
}
