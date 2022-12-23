#ifndef cook
#define cook

// Inform the server that it can start serving
sem_t* sem_server;

// Inform the chef that it can rest
sem_t* sem_cook;

// Mutual exclusion of the shelf
sem_t* sem_shelf;

void init_semaphores();

void destroy_semaphores();


#endif /* cook */
