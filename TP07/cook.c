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

int server_sem (int current_pizzas) {
    if current_pizzas == 0 {
        sem_wait(sem_server);
    }
    else {
        sem_post(sem_server);
    }
}

// Inform the chef that it can rest
sem_t* sem_cook;

int cook_sem (int current_pizzas) {
    if current_pizzas == MAX_PIZZAS {
        sem_wait(sem_cook);
    }
    else {
        sem_post(sem_cook);
    }
}

// Mutual exclusion of the shelf
sem_t* sem_shelf;

int shelf_sem (int current_pizzas) {
    if current_pizzas == 3 {
        sem_wait(sem_shelf);
    }
    else {
        sem_post(sem_shelf);
    }
}




