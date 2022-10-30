#include <dirent.h>
#include <errno.h>
#include <limits.h>  //PATH_MAX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  //snprintf
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include "util.h"
#include "optprsr.h"
#include "copy.h"

//* ST for 'STATE'
#define ST_JUST_LIST (0)      //! 000000 = 0

//! 00001 = 1
#define ST_PRESERVE_LINKS (1 << 0) //? -a was passed Links are copied as Links and resolved to ensure that both points to the same inode 

//! 00010 = 2
#define ST_MODIF_PERM (1 << 1) //? -f was passed. Permissions of files in destination are changed even if the weren't replaced
//* both -a -f passed => +3 (coherent with value returned by parseOptArgs)

//! 000100 = 4
#define ST_2FILES  (1 << 2)//? only 2 files are given as argument both exists

//! 001000 = 8
#define ST_2FILES_CREATE (1 << 3)//? only 2 files and destination does not exist  

//! 010000 = 16
#define ST_1FILE_1DIR (1 << 4)//? Only copying 1 file into 1 dir

//! 100000 = 32
#define ST_MIX (1 << 5)


int main(int argc, char* argv[]) {
    int x1 = 1 << 1;
    
    int x2 = 1 << 2;
    
    int state = 0;
    state += ST_2FILES;
    
    int s2 = 0;
    s2 |= ST_MIX | ST_PRESERVE_LINKS;

    /* printf("%d \n", (0b011101 >> 1));
    printf("%d \n", (0b000110 >> 3));
    printf("%d \n", (0b000111 >> 2)); */
    printf("%d\n", (s2 & ST_PRESERVE_LINKS));
    printf("%d\n", (s2 & ST_MODIF_PERM));



    return 0;
}
