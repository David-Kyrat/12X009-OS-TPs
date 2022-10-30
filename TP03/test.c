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

#define ST_JUST_LIST (0)      //! 000000 = 0

//? -a was passed Links are copied as Links and resolved to ensure that both points to the same inode 
#define ST_PRESERVE_LINKS (1 << 0) //! 00001 = 1

//? -f was passed. Permissions of files in destination are changed even if the weren't replaced
#define ST_MODIF_PERM (1 << 1)     //! 00010 = 2
//* both -a -f passed => +3 (coherent with value returned by parseOptArgs)

//? only 2 files are given as argument both exists
#define ST_2FILES  (1 << 2)        //! 000100 = 4

//? only 2 files and destination does not exist
#define ST_2FILES_CREATE (1 << 3)  //! 001000 = 8

//? multiple folders
#define ST_MULT_FOLDER (1 << 4)    //! 010000 = 16


int main(int argc, char* argv[]) {
    int x1 = 1 << 1;
    
    int x2 = 1 << 2;
    
    int state = 0;
    state += ST_2FILES;
    
    int s2 = 0;
    s2 |= ST_2FILES | ST_MODIF_PERM | ST_PRESERVE_LINKS;

    printf("%d \n", (0b011101 >> 1));
    printf("%d \n", (0b000110 >> 3));
    printf("%d \n", (0b000111 >> 2));


    return 0;
}
