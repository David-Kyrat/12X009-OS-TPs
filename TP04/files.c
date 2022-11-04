#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>  //snprintf
#include <limits.h>  //PATH_MAX
#include <dirent.h>  // opendir
#include <sys/stat.h>   // open
#include <sys/types.h> //  idem
#include <fcntl.h>    //   idem
#include <unistd.h>  //    close, stat 
#include <time.h>

#include "util.h"
#include "files.h"

int isDot(const char *entry_name) {
    return strcmp(entry_name, "..") == 0 || strcmp(entry_name, ".") == 0;
}

const char permRepr[] = {'r', 'w', 'x'};


int exists(const char* path) {
    int exist = access(path, F_OK);
    
    // If real error happened (ENOENT is not an error because we do want to check for existence or non-existence)
    if (exist < 0 && errno != ENOENT) {
        int savedErr = errno;
        fprintf(stderr, "Cannot check existance of \"%s\" : %s.\n", path, strerror(savedErr));
        return -1;
    }
    return exist == 0;
}


int concat_path(char buf[], const char* parent, const char* current) {

    // computes the name of the subdirectory and checks if it is not too long
    if (snprintf(buf, PATH_MAX, "%s/%s", parent, current) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }

    return EXIT_SUCCESS;
}


int isFile(const char* path, int lstat) {
    struct stat infos = lstat ? lstat_s(path) : stat_s(path);
    return S_ISREG(infos.st_mode);
}

int isDir(const char* path) {
    struct stat infos = stat_s(path);
    return S_ISDIR(infos.st_mode);
}


const char* getFileName(const char* path) {
    const char* absPath = realpath(path, NULL);
    if (absPath == NULL) {
        int savedErr = errno;
        fprintf(stderr, "%s, Cannot get fileName: %s\n", path, strerror(savedErr));
        return NULL;
    }
    int len = strlen(absPath)-1;

    while(absPath[len] != '/' && len >= 0) len--;
    //* Now absPath[len] points to first '/' in absPath (starting from the end)
    const char* out = &(absPath[len+1]); //extract a view on a sublist of absPath. (i.e. pointer to some element in it)
    return out;
}

const char* absPath(const char* path) {
    const char* out = realpath(path, NULL);
    if (out == NULL) {
        int savedErr = errno;
        fprintf(stderr, "%s, Cannot get absolute-path: %s\n", path, strerror(savedErr));
        return NULL;
    }
    return out;
}
