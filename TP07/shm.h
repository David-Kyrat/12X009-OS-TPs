#ifndef _SHM_H
#define _SHM_H

#include <stdlib.h>
#include <stdio.h>

//La mémoire partagée contiendra deux valeur:
//- un indicateur stipulant si un autre processus est près pour l'opération
//- un nombre qui sera incémenté par les deux processus conjointement
typedef struct {
    char isReady;
    long int number;
} sharedMemory;

#define READY 0
#define MAX_PIZZAS 10

#endif
