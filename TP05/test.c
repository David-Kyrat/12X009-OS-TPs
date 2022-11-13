#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "optprsr.h"
#include "functions.h"




int main(int argc, char* argv[]) {
    puts(" ");


    const char* t1;
    t1 = strdup("lul");

    printf("new value %s\n", t1);
    /*printf("test addr:\t %p\t %p\n", &t1, t1); */

    //int x= checkArgsNb(argc);
    //printf("checkArgsNb %d\n", x);

    return 0;
}
