#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

#define switch_str(pattern, cases, caseNb) switch(strswitch(pattern, cases, caseNb))



int main(int argc, char* argv[]) {
    const char* patt = "cd";
/*     char* cases[] = {"xdfe", "abcde","cd"};
    int match = strswitch(patt, cases, 11);
    printf("match: %d\n", match); */
    const char* test = "a,b,c,d,e";
    int size;
    /* const char SEP = ',';
    const char** split = strsplit(test, &SEP, &size); */

    arrPrint(split, size);
    return 0;
}

