#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "shm.h"



int main (int argc, char* argv[]) {

    int fd, i;
    sharedMemory *shm;

    // Create a read/write shared memory
    if((fd = shm_open(argv[1], O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR)) == -1)
        fprintf(stderr, "shm_open");

    // Size of memory = indicator + number
    if(ftruncate(fd, sizeof(sharedMemory)) == -1)
        fprintf(stderr, "ftruncate");

    // File mapping (The parameters must correspond to the opening mode of the POSIX object)
    shm = mmap(NULL, sizeof(sharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(shm == MAP_FAILED)
        fprintf(stderr, "mmap");

    // Initialization memory to zero
    shm->number = 0;

    // We wait for the other process to work
    while(shm->isReady != READY)
        sleep(1);

    // Unlink the shared memory
    shm_unlink(argv[1]);

    // Start counting pizzas
    for(i = 0; i < MAX_PIZZAS; i++)
        shm->number = shm->number + 1;
        printf("Number of pizzas: %ld", shm->number);

    // Unmap
    if(munmap(shm, sizeof(sharedMemory)) == -1)
        fprintf(stderr, "munmap");

    return 0;

}


