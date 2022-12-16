#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "shm.h"


int main(int argc, char* argv[])
{
    int fd, i;
    sharedMemory *shm;

    //Vérifier les entrées
    if(argc != 2)
    {
        printf("Usage: shm-create /sharedMemoryName\n\n");
        exit(0);
    }

    //Créer une mémoire  partagée en lecture / écriture
    if( (fd = shm_open(argv[1], O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR)) == -1)
        OnError("shm_open");

    //Taille de la mémoire = indicator + number
    if( ftruncate(fd, sizeof(sharedMemory)) == -1)
        OnError("ftruncate");

    //File mapping (Les parametres doivent correspondres au mode d'ouverture de l'objet POSIX)
    shm = mmap(NULL, sizeof(sharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(shm == MAP_FAILED)
        OnError("mmap");
    printf("Adr memoire partagée:%p\n", shm);

    //Initialization memoire a zero
    shm->number = 0;

    //On attend l'autre processus pour travailler
    shm->isReady = !READY;
    printf("Ok j'attends un processus qui peut m'aider...\n");
    while(!(shm->isReady == READY));

    //On peut desassocier l'objet (retirer l'inode) car l'objet ne sera supprimé que lors du munmap
    shm_unlink(argv[1]);

    //Ok on travail
    for(i=0;i<NUM_INCREMENTS;i++)
        shm->number = shm->number + 1;

    //Afficher le résultat
    printf("Pour moi %d, le total est %ld\n", getpid(), shm->number);

    //Unmap
    if(munmap(shm, sizeof(sharedMemory)) == -1)
        OnError("munmap");

    return 0;
}

