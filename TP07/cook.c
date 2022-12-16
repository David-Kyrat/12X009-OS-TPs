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



