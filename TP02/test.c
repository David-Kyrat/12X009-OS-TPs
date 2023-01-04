#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash_calc.h"

int main(int argc, char* argv[]) {
    const char* path1 = "res/string-tohash.txt";
    const char* path2 = "res/test.tx";

    const char* file = path1;
    printf("Extracting content of %s.\n", file);

    const char* fileContent = convert_f_to_s(file);
    if (fileContent == NULL) {
        perror("content is NULL");
        exit(1);
    }

    printf("Content of %s is:\n\t %s\n", file, fileContent);

    return 0;
}
