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