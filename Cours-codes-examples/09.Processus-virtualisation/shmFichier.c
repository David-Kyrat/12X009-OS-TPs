#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

//Changer ce paramètre pour observer le copy-on-write
//#define MAP_METHOD MAP_PRIVATE
#define MAP_METHOD MAP_SHARED

void OnError(const char* str) { perror(str); exit(1); }

int main(int argc, char* argv[])
{
    int fd, i;
    char *fileContent;
    struct stat fileStat;
    pid_t pid;

    //Vérifier les entrées
    if(argc != 2)
    {
        printf("Usage: shmFichier filename\n\n");
        exit(0);
    }

    printf("mon PID est: %d\n", getpid());

    //Ouvrir le fichier en lecture / écriture
    if( (fd = open(argv[1], O_RDWR)) == -1)
        OnError("open");

    //Determiner le taille de la mémoire necessaire pour mapper tout le fichier
    if( fstat(fd, &fileStat) == -1)
        OnError("fstat");

    //Attente entrée clavier pour regarder lespace mémoire du processus (commande pmap)
    printf("Attente clavier avant le mmap du père");
    getchar();

    //File mapping (Les parametres doivent correspondres au mode d'ouverture du fichier)
    fileContent = (char*) mmap(NULL, (size_t) fileStat.st_size, PROT_READ | PROT_WRITE, MAP_METHOD, fd, 0);
    if(fileContent == MAP_FAILED)
        OnError("mmap");

    //Fermer le fichier - possible car mmap crée une nouvelle référence au fichier
    close(fd);

    //Attente entrée clavier pour regarder l'espace mémoire du processus (commande pmap)
    printf("Attente clavier après le mmap du père");
    getchar();


    //Fork: le père lit et ecrit sur STDOUT le fils modifie le contenu
    //Après un fork les deux processus on le même espace d'adressage virtuel
    if( (pid = fork()) <0)
        OnError("fork");

    if(pid == 0) //fils
    {
        //Editer le contenu de la mémoire partagée
        for(i=0;i<fileStat.st_size;i++)
            fileContent[i] = '0';
    }
    else //père
    {
        //Afficher le contenu de la mémoire partagée
        for(i=0;i<fileStat.st_size;i++)
            printf("%c",fileContent[i]);
        printf("\n");

        //Attente du fils
        wait(NULL);
    }

    //Unmap
    if(munmap(fileContent, fileStat.st_size) == -1)
        OnError("munmap");

    return 0;
}

