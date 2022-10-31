#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>  //PATH_MAX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  //snprintf
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "copy.h"
#include "optprsr.h"
#include "util.h"

//* ST for 'STATE'
#define ST_JUST_LIST (0)  //! 000000 = 0

//! 00001 = 1
#define ST_PRESERVE_LINKS (1 << 0)  //? -a was passed Links are copied as Links and resolved to ensure that both points to the same inode

//! 00010 = 2
#define ST_MODIF_PERM (1 << 1)  //? -f was passed. Permissions of files in destination are changed even if the weren't replaced
//* both -a -f passed => +3 (coherent with value returned by parseOptArgs)

//! 000100 = 4
#define ST_2FILES (1 << 2)  //? only 2 files are given as argument both exists

//! 001000 = 8
#define ST_2FILES_CREATE (1 << 3)  //? only 2 files and destination does not exist

//! 010000 = 16
#define ST_1FILE_1DIR (1 << 4)  //? Only copying 1 file into 1 dir

//! 100000 = 32
#define ST_MIX (1 << 5)

/**
 * Computes actual name of a file. This function does a malloc, rembember to free result when done!
 * @param path path to file to relove name from.
 * @return char* Actual resolved name of file located at path. (Use real path to get the absolute path, then extract the name).
 */
char* getFileName(char* path) {
    char* absPath = realpath(path, NULL);
    if (absPath == NULL) {
        int savedErr = errno;
        fprintf(stderr, "%s, Cannot get fileName: %s\n", path, strerror(savedErr));
        return NULL;
    }
    int len = strlen(absPath) - 1;
    while (absPath[len] != '/' && len >= 0) len--;
    //* Now absPath[len] points to first '/' in absPath (starting from the end)

    char* out = &(absPath[len + 1]);  //extract a view on a sublist of absPath. (i.e. pointer to some element in it)
    return out;
}

int main(int argc, char* argv[]) {
    char* path = argv[1];
    printf("path: %s\n", path);
    char* absPath = realpath(path, NULL);
    printf("realPath: %s\n", absPath);
    char* actualFileName = getFileName(path);
    printf("\t Actual fileName: %s\n", actualFileName);

    return 0;
}
