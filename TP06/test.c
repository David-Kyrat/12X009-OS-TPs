#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>

#include "util.h"

typedef long long llong;

void arrPrint(char** arr, int size) {
    printf("[");
    for (int i = 0; i < size-1; i++)
        printf("\'%s\',", arr[i]);
    printf("\'%s\']\n", arr[size-1]);
}


int main(int argc, char* argv[]) {
   

    return 0;
}

