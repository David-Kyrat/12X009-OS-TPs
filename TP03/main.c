#include <dirent.h>
#include <errno.h>
#include <limits.h>  //PATH_MAX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  //snprintf
#include <sys/stat.h>
#include <sys/types.h>
#include "util.h"
#include "optprsr.h"


/** @return Is 'entry_name' '.' or '..' ? */
int isDot(const char *entry_name) {
    return strcmp(entry_name, "..") == 0 || strcmp(entry_name, ".") == 0;
}

char permRepr[] = {'r', 'w', 'x'};

/**
 * Takes the mode (int) and dtype (int) of an inode and returns a string representing the permissions of the file
 * 
 * @param mode the mode of the file,
 * 
 * @return Permissions of the file, as a String. (i.e. -rw-r--r-x ...)
 */
char* computePerm(int mode) {
      //* 3 groups: owner, group, others, 3 perm: read write execute
    int x = 01, r = 04, w = 02, permNb = 3, dashNb = 10;
    int perms[] = {r, w, x};
    char permsPrty[10];  // Permission as a readable string
    for (int i = 0; i < dashNb; i++) permsPrty[i] = '-';

    char typeLetter = '-';
    if (S_ISDIR(mode)) typeLetter = 'd';
    else if (S_ISLNK(mode)) typeLetter = 'l';
    else if (S_ISCHR(mode)) typeLetter = 'c';
    else if (S_ISBLK(mode)) typeLetter = 'b';
    else if (S_ISFIFO(mode)) typeLetter = 'p';
    else if (S_ISSOCK(mode)) typeLetter = 's';
    permsPrty[0] = typeLetter;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < permNb; j++) {
            int crtPerm = mode & (perms[j] << (2-i) * permNb);
            if (crtPerm) permsPrty[i * 3 + j + 1] = permRepr[j];
            // i*3 to shift to corresponding block and + 1 because permPrty[0] is d_type
        }
    }
    return strndup(permsPrty, dashNb);
}

/**
 * Emulates ls -lRa behavior. (except that it doesnt print . and .. because they are always present).  
 * 
 * It Takes a directory name as an argument, opens it, reads its entries, and recursively calls itself
 * on each subdirectory.
 * 
 * @param dir_name the name of the directory to list
 * @return Error code. (0 if success else see errno.h for more info)
 */
static int list_dir(const char *dir_name) {
    //TODO: CHECK IF IS FILE, BASE ON INODE INFO
    int err = errno;
    DIR *d = opendir(dir_name);
    struct dirent *entry;
    const char *d_name;  // name of entry

    // In case of exception on openning
    if (!d) {
        err = errno;
        fprintf(stderr, "Cannot open directory '%s': %s\n", dir_name, strerror(err));
        return err;
    }
    printf("%s/:\n", dir_name);

    // Loop on each entry
    while ((entry = readdir(d)) != NULL) {
        // Get entry name
        d_name = entry->d_name;
        int isEntDot = isDot(d_name);  //* is 'entry' '..' or '.' ?
        char path[PATH_MAX]; 

        //* Do not print ., .. as they are always here
        if (!isEntDot) {
            struct stat infos;
            // computes the name of the subdirectory and checks if it is not too long
            if (snprintf(path, PATH_MAX, "%s/%s", dir_name, d_name) >= PATH_MAX) {
                fprintf(stderr, "Path length has gotten too long.\n");  // Check that subdir pathname isnt too long
                return ENAMETOOLONG;//exit(ENAMETOOLONG);
            }
            if (lstat(path, &infos) < 0) fprintf(stderr, "Cannot stat %s: %s\n", d_name, strerror(errno));
            
            char* permissions = computePerm(infos.st_mode);
            printf(" %s %*ld   %s/%s\n", permissions, 13, infos.st_size, dir_name, d_name);
        }

        // Is 'entry' a subdirectory ?
        if (entry->d_type & DT_DIR) {
            if (!isEntDot) {
                printf("\n");
                err = list_dir(path);
                printf("\n%s/ (rest):\n", dir_name);
            }
        }
    }
    // closing directory
    if (closedir(d)) {
        fprintf(stderr, "Could not close '%s': %s\n", dir_name, strerror(errno));
        exit(EXIT_FAILURE);
    }

    return err;
}

int main(int argc, char *argv[]) {
    //list_dir("/var/log/");
    int fileNb = -1, err = 0;
    char** files = parseArgs(argc, argv, &fileNb);
    if (!files || fileNb <= 0) {
        int errno_cpy = errno;
        fprintf(stderr, "%s: %s\n\t - Usage: %s folder1 folder2/ destination \n\n", argv[0], strerror(errno_cpy), argv[0]);
        return errno_cpy;
    }

    for (int i = 0; i < fileNb; i++) {
        err = list_dir(files[i]);
        //if (err != 0) fprintf(stderr, "Error at file %d, %s : %s\n", i, files[i], strerror(err));
        printf("    ____________________  \n\n\n");
    }
    
    return err;
}

