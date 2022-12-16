#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "shm.h"


// CODE FROM COURSE


int main(int argc, char* argv[]) {

    int fd, i;
    sharedMemory *shm;

    //Vérifier les entrées
    if(argc != 2)
    {
        printf("Usage: shm-help /sharedMemoryName\n\n");
        exit(0);
    }

    //Ouvrir une mémoire  partagée en lecture / écriture
    if( (fd = shm_open(argv[1], O_RDWR, S_IRUSR | S_IWUSR)) == -1)
        OnError("shm_open");

    //File mapping (Les parametres doivent correspondres au mode d'ouverture de l'objet POSIX)
    shm = mmap(NULL, sizeof(sharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(shm == MAP_FAILED)
        OnError("mmap");
    printf("Adr memoire partagée:%p\n", shm);

    //On signal que l'on est prêt
    shm->isReady = READY;

    //Ok on travail
    for(i=0;i<NUM_INCREMENTS;i++)
        shm->number = shm->number + 1;

    //Unmap
    if(munmap(shm, sizeof(sharedMemory)) == -1)
        OnError("munmap");

    return 0;
}

