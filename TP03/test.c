#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  //snprintf
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>

#include "util.h"
#include "files.h"

#define ST_JUST_LIST (0)            //! 000000 = 0
#define ST_PRESERVE_LINKS (1 << 0)  //? -a was passed Links are copied as Links and resolved to ensure that both points to the same inode
#define ST_MODIF_PERM (1 << 1)      //? -f was passed. Permissions of files in destination are changed even if the weren't replaced
#define ST_2FILES (1 << 2)          //? only 2 files are given as argument both exists
#define ST_2FILES_CREATE (1 << 3)   //? only 2 files and destination does not exist
#define ST_1FILE_1DIR (1 << 4)      //? Only copying 1 file into 1 dir
#define ST_MIX (1 << 5)

//! if (create_intermed) then create missing folder in to i.e. in /foo/bar/to => will create /foo and /foo/bar if they do not exists
//! dest **has** to exist if create_intermed is true
int create_subpaths_ifneeded(const char* from, const char* to, const char* dest) {
    //In copy folders dest has exist so we can resolve abs path of dest then append rest
    //printf("from: %s,\n to:%s,\n dest:%s\n\n", from, to, dest);
    if (!exists(dest)) {
        errno = ENOENT;
        fprintf(stderr, "Cannot copy %s to %s, Dest: %s: %s\n", from, to, dest, strerror(ENOENT));
        return -1;
    }
    
    int destlen = strlen(dest);
    int path_after_len = strlen(to) - destlen;
    char* abs_dest = absPath(dest); //abs_dest for sure exists => will try to locate folders from this path
    int abs_dest_len = strlen(abs_dest);

    //if dest is not parent dir of 'to'
    if (strstr(to, dest) == NULL) {
        errno = EINVAL;
        fprintf(stderr, "Cannot copy %s : %s is not a parent of %s\n", from, dest, to);
        return -1;
    }

    //extract where dest and to differ (we have that 'dest' is included in 'to')
    char* path_after = &to[destlen+1];
    //printf("dest:%s, destlen: %d,  to:%s, strlen(to): %d, strlen(path_after): %d \n", dest, destlen, to, strlen(to), path_after_len);

    const char* path = calloc(PATH_MAX, sizeof(char));

    if (concat_path(path, abs_dest, path_after) < 0) {
        hdlCatErr(to);
        return -1;
    }
    int path_len = strlen(path);
    //printf("abs_dest: %s,\n path_after:%s,\n path:%s,\n", abs_dest, path_after, path);
    printf("len:%d, Path: %s\n", path_len, path);

    int* sp_idxs = calloc(path_len, sizeof(int)); //indices of each end of subpath i.e. 4 and 8  for /foo/bar/to
    int sp_len = 0;
    
    for (int i = abs_dest_len+1; i < path_len; i++) {
        if (path[i] == '/') {
            if (sp_len == -1 && 0) sp_len == 0;
            else {
                sp_idxs[sp_len++] = i;
            }
        }
    }
    //* Iterate on each subpath and create them if they do not exists
    for (int i = 0; i < sp_len; i++) {
        //printf("arr[%d] = %d\n", i, sp_idxs[i]);
        char *crt_subpath = strsub(path, sp_idxs[i]);
        if (!exists(crt_subpath)) {
            struct stat info = stat_s(from);
            if (mkdir(crt_subpath, 0700) < 0) {
                int savedErr = errno;
                fprintf(stderr, "Cannot create %s: %s\n", crt_subpath, strerror(savedErr));
            } else printf("-- Created %s -- \n\n", crt_subpath);
            
        }
        //printf("crt_subpath: %s\n", crt_subpath);
    }

    return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {
    /* char* path = argv[1];
    char* absPath = realpath(path, NULL);
    const char* p1 = absPath;
    printf("path1: %s\n", p1);
    const char* p2 = realpath(argv[2], NULL);
    printf("path2: %s\n", p2);
    int m = is_modified(p1, p2);
    char* msg = m == -1 ? "older" : (m == 0 ? "equal" : "newer");
    printf("%s %s %s\n", path, msg, argv[2]); */
    create_subpaths_ifneeded(argv[1], argv[2], argv[3]);


    return 0;
}
