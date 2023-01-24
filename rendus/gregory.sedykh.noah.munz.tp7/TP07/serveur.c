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


// 3 semaphores

// Inform the server that it can start serving
sem_t* sem_server;

// Inform the chef that it can rest
sem_t* sem_cook;

// Mutual exclusion of the shelf
sem_t* sem_shelf;


// Initialize the semaphores
void init_semaphores() {
    sem_server = sem_open("sem_server", 0666, 0);
    sem_cook = sem_open("sem_cook", 0666, 0);
    sem_shelf = sem_open("sem_shelf", 0666, 1);
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


/* Handles the server's job of serving pizzas */
void serve_pizza(sharedMemory *shm, int i){

        // Wait for new pizza to be available
        sem_wait(sem_server);
        
        // Serve a pizza within 5 seconds
        sleep(rand() % 5 + 1);

        // Remove a pizza from the shelf
        shm->number--;
        printf("Server served a pizza. Pizzas left on shelf: %ld\n", shm->number);
        sem_post(sem_shelf);

        // Tell the chef to start cooking again once there less than 3 pizzas on the shelf
        if (shm->number < 3) {
            sem_post(sem_cook);
        }

        // Once we have served 10 pizzas, stop
        if (i == 9) {
            printf("Server has served all the pizzas.\n");
        }
    }



int main(int argc, char* argv[]) {

    int fd;
    sharedMemory *shm;

    // Open the read/write shared memory
    if((fd = shm_open("pizzaShelf", O_RDWR, S_IRUSR | S_IWUSR)) == -1){
        perror("Failed to open shared mem");
        exit(EXIT_FAILURE);
    }

    // File mapping
    shm = mmap(NULL, sizeof(sharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(shm == MAP_FAILED) {
        perror("Failed to map shared mem");
        exit(EXIT_FAILURE);
    }

    // Shared memory (server) is ready
    shm->isReady = READY;

    init_semaphores();

    printf("Starting to serve pizzas.\n");
    // Start counting pizzas
    for(int i = 0; i < MAX_PIZZAS; i++) {
        serve_pizza(shm, i);
    }

    destroy_semaphores();

    // Unmap the memory
    if(munmap(shm, sizeof(sharedMemory)) == -1) {
        perror("Failed to unmap shared mem");
        exit(errno);
    }

    return 0;
}

