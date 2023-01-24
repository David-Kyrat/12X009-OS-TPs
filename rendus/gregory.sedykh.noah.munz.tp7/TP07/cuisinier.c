#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <errno.h>

#include "shm.h"

// Inform the server that it can start serving
sem_t* sem_server;

// Inform the chef that it can rest
sem_t* sem_cook;

// Mutual exclusion of the shelf
sem_t* sem_shelf;


// Initialize the semaphores
void init_semaphores() {
    sem_server = sem_open("sem_server", O_CREAT, 0666, 0);
    sem_cook = sem_open("sem_cook", O_CREAT, 0666, 0);
    sem_shelf = sem_open("sem_shelf", O_CREAT, 0666, 1);
}

// Destroy the semaphores
void destroy_semaphores() {
    sem_close(sem_server);
    sem_close(sem_cook);
    sem_close(sem_shelf);
    sem_unlink("sem_server");
    sem_unlink("sem_cook");
    sem_unlink("sem_shelf");
}


/* Handles the chef's job of making pizzas */
void cook_pizza(sharedMemory *shm, int i){

        // Cook a pizza within 5 seconds
        sleep(rand() % 5 + 1);

        // Add the cooked pizza to the shelf
        sem_wait(sem_shelf);
        shm->number++;
        printf("Chef added a pizza to the shelf. Pizzas on shelf: %ld\n", shm->number);
        sem_post(sem_shelf);

        // Tell the server that there is a new pizza available
        sem_post(sem_server);

        // If there are 3 pizzas on the shelf, tell the chef to rest
        if (shm->number == 3) {
            printf("Chef is taking a break.\n");
            sem_wait(sem_cook);
        }

        if (i == 9) {
            printf("Chef is done cooking.\n");
            sem_post(sem_server);
        }   

}



int main (int argc, char* argv[]) {

    int fd;
    sharedMemory *shm;

    // Create a read/write shared memory
    if ((fd = shm_open("pizzaShelf", O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR)) == -1){
        perror("Failed to create shared mem");
        exit(EXIT_FAILURE);
    }

    // Size of memory = indicator + number
    if (ftruncate(fd, sizeof(sharedMemory)) == -1) {
        perror("Failed to truncate shared mem");
        exit(EXIT_FAILURE);
    }

    // File mapping (The parameters must correspond to the opening mode of the POSIX object)
    shm = mmap(NULL, sizeof(sharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shm == MAP_FAILED){
        perror("Failed to map shared mem");
        exit(EXIT_FAILURE);
    }

    // Initialize the shelf (memory) to zero
    shm->number = 0;

    // We wait for the server to work
    while(shm->isReady != READY)
        sleep(1);
    

    // Initialize the semaphores
    init_semaphores();

    printf("Starting to cook pizzas.\n");
    // Start counting pizzas
    for(int i = 0; i < MAX_PIZZAS; i++) {
        cook_pizza(shm, i);
    }

    // Destroy the semaphores
    destroy_semaphores();

    // Unmap the memory
    if(munmap(shm, sizeof(sharedMemory)) == -1)
        fprintf(stderr, "munmap");
    
    // Unlink the shared memory
    shm_unlink("pizzaShelf");

    return 0;

}


