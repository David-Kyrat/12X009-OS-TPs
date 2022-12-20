#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
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
    sem_server = sem_open("sem_server", O_CREAT, 0644, 0);
    sem_cook = sem_open("sem_cook", O_CREAT, 0644, 0);
    sem_shelf = sem_open("sem_shelf", O_CREAT, 0644, 1);
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




