#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

void* tryalc(void* allocReturn, int line) {
    if (allocReturn) return allocReturn;

    //*malloc returned null => exiting with error message ENOMEM
    //errno = ENOMEM;
    fprintf(stderr, "line %d: Cannot allocate Memory\n", line);
    exit(ENOMEM);
}


char* catArr(char** arr, int arrSize, char* sep) {
    char* out;
    int sepLen;
    if (sep) sepLen = strlen(sep);
    size_t size = arrSize + 1 + (sep ? sepLen * arrSize : 0);

    tryalc(out = (char*)malloc((size * sizeof(char*))), __LINE__);

    for (int i = 0; i < arrSize; i++) {
        char* crt = arr[i];

        if (sep && i != 0) strncat(out, sep, sepLen);
        strncat(out, crt, strlen(crt) + 1);
    }

    return out;
}

void test(char** arr, int arrSize) {
    
    for (int i = 0; i < arrSize; i++) {
        int n = strlen(arr[i]);
        char* cpy = malloc(n+1);
        cpy = strncpy(cpy, arr[i], n);

        printf("cpy = \"%s\"\n", cpy);
        free(cpy);
    }
}

int main() {

    char* arr[] = {"test1", "t2", "lul3", "lol4", "foo5"};

    test(arr, 5);
    puts("-----------------");

    char* cat = catArr(arr, 5, "-");
    printf("cat : %s\n", cat);

    return 0;
}
