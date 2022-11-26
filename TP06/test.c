#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

void arrPrint(char** arr, int size) {
    printf("[");
    for (int i = 0; i < size-1; i++)
        printf("\'%s\',", arr[i]);
    printf("\'%s\']\n", arr[size-1]);
}

int main(int argc, char* argv[]) {
    char* s = "lul1 lul2    lul3  a b  c d";
    int len = 0;

    char** spl = strsplit(s, " ", &len);
    arrPrint(spl, len);
/* 
    char* s1 = calloc(25, 1);
    for (int i = 0; i < 25; i++) s1[i] = '5';
    printf("%s\n", s1);

    s1[7] = '\0';
    printf("%s\n", s1);
    const char* s2 = &s1[8];
    printf("%s\n\n", s2);

    s1[5] = '\0';
    printf("%s\n", s1);
    printf("%c\n\n", s1[6]); */

    return 0;
}

/*
  char str1[] = "exit akwdkm mldasmlkd 2453";
    char str2[] = "cd ../../../";
    char str3[] = "k";
    char* strs[] = {str1, str2, str3};
    char pat1[] = "exit";
    char pat2[] = "cd";
    int sub = strlen(pat1);

    for (int i = 0; i < 3; i++) {
        int cmp = strncmp(strs[i], pat1, sub) == 0;
        printf("\"%s\"[0-%d] == \"%s\" ? %d\n", strs[i], sub, pat1, cmp);
    }
*/