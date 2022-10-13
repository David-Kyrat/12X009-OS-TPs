#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void* tryalc(void* allocReturn, int line) {
    if (allocReturn) return allocReturn;

    //*malloc returned null => exiting with error message ENOMEM
    fprintf(stderr, "line %d: Cannot allocate Memory\n", line);
    exit(-1);
}

char* catArr(char** arr, int arrSize, char* sep) {
    char* out;
    int sepLen = strlen(sep);
    size_t size = arrSize + 1 + (sep ? sepLen * arrSize : 0);

    tryalc(out = (char*) malloc((size * sizeof(char*))), __LINE__);

    for (int i = 0; i < arrSize; i++) {
        char* crt = arr[i];
        if (sep && i != 0) strncat(out, sep, sepLen);
        strncat(out, crt, strlen(crt) + 1);
    }
    return out;
}

void reassignArr(char*** arrToReassign) {
    char** t = malloc(5 * sizeof(char*));
    char* test[] = {"foo1", "foo2", "foo3", "foo4"};
    t = test;
    *arrToReassign = t;
}

void temp() {
    for (int i = 0; i < 5; i++) {
        printf("");
    }
    
}

int main() {

    //char* test[] = {"lul1", "lul2", "lul3", "lul4"};
    //char** t = malloc(5 * sizeof(char*));
    char*** t = malloc(5 * sizeof(char**));
    (*t)[0] = "lul1";
    (*t)[1] = "lul2";
    (*t)[2] = "lul3";
    (*t)[3] = "lul4";

    printf("test before [%s]\n", catArr(t, 4, ", "));
    //sreassignArr(&t);
    
    temp();
    printf("test after [%s]\n", catArr(t, 4, ", "));
    temp();
    printf("test after [%s]\n", catArr(t, 4, ", "));
    temp();
    printf("test after [%s]\n", catArr(t, 4, ", "));

    return 0;
}
