#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

// 0:default,  1:red, 2:Green,  3:Blue, 4:Purple, 5:yellow,  6:cyan,  7:grey
const char* colors[] = {"\033[0m", "\033[0;31m", "\033[0;32m", "\033[0;34m", "\033[0;35m", "\033[0;33m", "\033[0;36m", "\033[0;30m"};

void testColor(const char* code, int m) {
    printf("%s %d: Test lul %s\n", code, m, colors[0]);
}

int main(int argc, char* argv[]) {

    testColor(colors[7], 0);
    for (int i = 0; i < 10; i++) {
        char msg[100];
        sprintf(msg, "\033[0;0%dm", i);
        testColor(msg, i);
    }    
    for (int i = 10; i < 31; i++) {
        char msg[100];
        sprintf(msg, "\033[0;%dm", i);
        testColor(msg, i);
    }

    return 0;
}

