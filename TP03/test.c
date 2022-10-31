#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  //snprintf
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "util.h"

#define ST_JUST_LIST (0)            //! 000000 = 0
#define ST_PRESERVE_LINKS (1 << 0)  //? -a was passed Links are copied as Links and resolved to ensure that both points to the same inode
#define ST_MODIF_PERM (1 << 1)      //? -f was passed. Permissions of files in destination are changed even if the weren't replaced
#define ST_2FILES (1 << 2)          //? only 2 files are given as argument both exists
#define ST_2FILES_CREATE (1 << 3)   //? only 2 files and destination does not exist
#define ST_1FILE_1DIR (1 << 4)      //? Only copying 1 file into 1 dir
#define ST_MIX (1 << 5)

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

/**
 * Comparator of date. i.e. compare time 'ts2' and 'ts1' and return an integer (1, 0, -1)
 * describing if ts2 > ts1, ts2 == ts1 or if ts2 < ts1.
 
 * @param ts2 Second timestamp to compare
 * @param ts1 First timestamp to compare
 * 
 * @return 1, 0 or -1 if ts2 is respectively Greater, Equal to or Less than ts1.
 */
int dateCmpr(struct timespec ts2, struct timespec ts1) {
    const time_t t1 = ts1.tv_sec, t2 = ts2.tv_sec;
    const time_t elapsed = difftime(t2, t1);
    //return elapsed > 0 ? 1 : (elapsed == 0 ? 0 : -1);
    return elapsed < 0 ? -1 : (elapsed > 0);
    //? If elapsed < 0 then ts2 < ts1. Else (elapsed > 0) will return 1 Iif ts2 > ts0, and 0, if elapsed == 0. (because the only case left is elapsed == 0)
}

/**
 * Used when backing up files to check if 'src' has been modified since last backup at 'dest'.
 * 
 * @param src source file (potentially newer)
 * @param dest destination file (file to check against)
 * @return 1 if and only if src is newer, if dest does not exist or if sizes are different. Otherwise it returns 0
 */
int is_modified(const char* src, const char* dest) {
    if (!exists(dest)) return 1; //* If destination does not exists => src is obviously newer
    
    struct stat src_infos = stat_s(src), dest_infos = stat_s(dest); //struct timespec src_ts = src_infos.st_mtim, dest_ts = dest_infos.st_mtim;
    int cmprison = dateCmpr(src_infos.st_mtim, dest_infos.st_mtim);
    
    return cmprison > 0 || src_infos.st_size != dest_infos.st_size;
}

int main(int argc, char* argv[]) {
    char* path = argv[1];
    char* absPath = realpath(path, NULL);
    const char* p1 = absPath;
    printf("path1: %s\n", p1);
    const char* p2 = realpath(argv[2], NULL);
    printf("path2: %s\n", p2);
    int m = is_modified(p1, p2);
    char* msg = m == -1 ? "older" : (m == 0 ? "equal" : "newer");
    printf("%s %s %s\n", path, msg, argv[2]);

    return 0;
}
