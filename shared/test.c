#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "util.h"

int main(int argc, char* argv[]) {

    printf("Hello world!\n");
    int res;
    strToInt("5", 10, &res);
    printf("%d\n", res);
}


