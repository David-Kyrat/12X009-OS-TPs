#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>




int main(int argc, char* argv[]) {
    printf("%s\n", strerror(128-256));
    printf("%s\n", strerror(256-128));
    return 0;
}

