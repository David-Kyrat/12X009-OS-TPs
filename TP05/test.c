#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "util.h"
#include "optprsr.h"
#include "functions.h"

int main(int argc, char* argv[]) {
    puts(" ");


    const char* t1;
    t1 = strdup("lul");

    printf("new value %s\n", t1);
    errno = 1;
    printRErr("%s, Cannot open given socket.\n", "");
    /*printf("test addr:\t %p\t %p\n", &t1, t1); */

    //int x= checkArgsNb(argc);
    //printf("checkArgsNb %d\n", x);

    return 0;
}
