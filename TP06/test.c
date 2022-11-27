#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

#define switch_str(pattern, cases, caseNb) switch(strswitch(pattern, cases, caseNb))

void arrPrint(char** arr, int size) {
    printf("[");
    for (int i = 0; i < size-1; i++)
        printf("\'%s\',", arr[i]);
    printf("\'%s\']\n", arr[size-1]);
}



int main(int argc, char* argv[]) {
    const char* patt = "cd";
    char* cases[] = {"xdfe", "abcde","cd"};
    int match = strswitch(patt, cases, 11);
    printf("match: %d\n", match);

    return 0;
}

