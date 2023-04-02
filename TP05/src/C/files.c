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

ssize_t getFileSize(const char* path) {
    struct stat infos = stat_s(path);
    ssize_t out = infos.st_size;
    if (out < 0) return -1; // st_size was set to value < 0 ==> way of telling us that an error happened in stat_s
    return out;
}

/**
 * Rread file and returns its content
 * 
 * @return Content of 'fileToRead' (returned string is malloc'd => remember to free().)
 */
char* buffread(const char* fileToRead) {
    int fd;
    char buff[4096];  // size of a block
    ssize_t readNb;   // number of bytes read
    size_t readTotNb = 0;
    char* bBuff; //big buffer which will contain all file and be realloc'd at each iteration

    // lstat the source file/folder
    struct stat infos = lstat_s(fileToRead);

    // Error handling if unable to open source in read-only or the destination in write-only, create or excl (error if create but file already exists)
    if ((fd = open(fileToRead, O_RDONLY)) < 0) {
        hdlOpenErr(fileToRead, 0);
        return NULL;
    }

    // Check that it's a regular file or link
    if (!(S_ISREG(infos.st_mode) || S_ISLNK(infos.st_mode))) {
        errno = EINVAL;
        fprintf(stderr, "\"%s\": Wrong file type\n", fileToRead);
        return NULL;
    }
    
    //* While there are bytes left to be read, reads them 4096 by 4096 (4096 or current size of 'buff' if its not that)
    //* nb: if readNb is < to what we expected we dont really care because the program will retry until having read everything
    while ((readNb = read(fd, buff, sizeof buff)) > 0) {
        char* crtToCopy = &buff[readTotNb];  // pointer to start of current 'portion of data' to copy
        readTotNb += readNb;
        //update allocated memory to the size of whats been read until now.
        bBuff = realloc(bBuff, (readTotNb+1)*sizeof(char));
        //copy at most readNb bytes fileToRead buff to bBuff i.e. append current content of the small buffer 'buff'.
        strncat(bBuff, crtToCopy, readNb);
    }
    if (readNb < 0) {
        hdlReadErr(fileToRead, 0, 1, fd);
        return NULL;
    }

    if (close(fd) < 0) {
        hdlCloseErr(fileToRead, 0);
        return NULL;
    }

    return bBuff;
}
