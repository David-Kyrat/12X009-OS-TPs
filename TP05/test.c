#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "optprsr.h"
#include "functions.h"

int main(int argc, char* argv[]) {
    puts(" ");

    short t = 1;
    printf("size of t: %ld \n", sizeof(t));
    //int x= checkArgsNb(argc);
    isPortValid(4);
    //printf("checkArgsNb %d\n", x);


    return 0;
}
