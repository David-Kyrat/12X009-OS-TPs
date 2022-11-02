#include <stdio.h>
#include <stdlib.h>

#include "util.h"
#include "optprsr.h"
#include "files.h"

int main() {

    printf("Hello World\n");

    int ex = exists("main.c");
    if (ex < 0 ) fprintf(stderr, "you fukced up\n");
    if (ex == 0) printf("still fucked up\n");
    else printf("Sucess\n");

    return 0;
}
