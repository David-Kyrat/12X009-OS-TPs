#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>  //snprintf
#include <limits.h>  //PATH_MAX
#include <dirent.h>  // opendir
#include <sys/types.h> //  open
#include <sys/stat.h>  //  idem
#include <fcntl.h>    //   idem
#include <unistd.h>  //    close, stat 
#include <time.h>

#include "util.h"
#include "copy.h"
#include "files.h"
#pragma clang diagnostic ignored "-Wincompatible-pointer-types-discards-qualifiers"

int isDot(const char *entry_name) {
    return strcmp(entry_name, "..") == 0 || strcmp(entry_name, ".") == 0;
}

const char permRepr[] = {'r', 'w', 'x'};

char* computePerm(int mode) {
      //* 3 groups: owner, group, others, 3 perm: read write execute
    int x = 01, r = 04, w = 02, permNb = 3, dashNb = 10;
    int perms[] = {r, w, x};
    char permsPrty[10];  // Permission as a readable string
    for (int i = 0; i < dashNb; i++) permsPrty[i] = '-';

    switch(mode & S_IFMT) {  // Bitwise OR to get only the bits related to file type (code taken from the manual)
        case S_IFDIR : permsPrty[0] = 'd';        break;
        case S_IFLNK : permsPrty[0] = 'l';        break;
        case S_IFCHR : permsPrty[0] = 'c';        break;
        case S_IFBLK : permsPrty[0] = 'b';        break;
        case S_IFIFO : permsPrty[0] = 'p';        break;
        case S_IFSOCK: permsPrty[0] = 's';        break;
    }

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < permNb; j++) {
            int crtPerm = mode & (perms[j] << (2-i) * permNb);
            if (crtPerm) permsPrty[i * 3 + j + 1] = permRepr[j];
            // i*3 to shift to corresponding block and + 1 because permPrty[0] is d_type
        }
    }
    return strndup(permsPrty, dashNb);
}


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


int listEntry(const char* path, struct stat infos) {
    int err = 0;
    char* permissions = computePerm(infos.st_mode);

    // Save the last modification time given
    time_t mtime = infos.st_mtime;

    // Initialize where the formatted date will be written
    char modif_time[100]; 

    // Initialize the time_info struct to manipulate the date
    struct tm* time_info;

    // Write the time as the local time of the computer
    time_info = localtime(&mtime);
    // Copy the formatted time to modif_time
    strftime(modif_time, 100, "%c", time_info);
        
    // Output the info
    printf(" %s %*ld      %s  %s\n", permissions, 13, infos.st_size, modif_time, path);
    return err;
}

int listEntryNoIn(const char* path) {
    return listEntry(path, lstat_s(path));
}


/**
 * Emulates ls -lRa behavior. (except that it doesnt print . and .. because they are always present).  
 * 
 * It takes a directory name as an argument, opens it, reads its entries, and recursively calls itself
 * on each subdirectory.
 * 
 * @param dir_name the name of the directory to list
 * @param copy int, if non-zero will backup files to destination
 * 
 * @return Error code. (0 if success else error code see errno.h for more info)
 */
int list_dir(const char *dir_name) {
    int err = errno;
    DIR *d = opendir(dir_name);
    struct dirent *entry;
    const char *d_name;  // name of entry

    // In case of exception on opening
    if (!d) return hdlOpenErr(dir_name, 0);

    //prints which dir is being listed
    printf("%s/:\n", dir_name);

    // Loop on each entry
    while ((entry = readdir(d)) != NULL) {
        // Get entry name
        d_name = entry->d_name;
        int isEntDot = isDot(d_name);  //* is 'entry' '..' or '.' ?
        const char path[PATH_MAX];

        //* Do not print ., .. as they are always here
        if (!isEntDot) {
            
            struct stat infos;
            // computes the name of the subdirectory and checks if it is not too long

            if(concat_path(path, dir_name, d_name) < 0) return hdlCatErr(d_name);

            if (lstat(path, &infos) < 0) fprintf(stderr, "Cannot stat %s: %s\n", d_name, strerror(errno));
            
            char* permissions = computePerm(infos.st_mode);

            // Save the last modification time given
            time_t mtime = infos.st_mtime;

            // Initialize where the formatted date will be written
            char modif_time[50];

            // Initialize the time_info struct to manipulate the date
            struct tm* time_info;

            // Write the time as the local time of the computer
            time_info = localtime(&mtime);

            // Copy the formatted time to modif_time
            strftime(modif_time, 50, "%c", time_info);

            // Output the info
            printf(" %s %*ld      %s  %s/%s\n", permissions, 13, infos.st_size, modif_time, dir_name, d_name);
            //* computes the name of the subdirectory and checks if it is not too long
            if (concat_path(path, dir_name, d_name) < 0) return hdlCatErr(d_name);
            if (lstat(path, &infos) < 0) fprintf(stderr, "Cannot stat %s: %s\n", d_name, strerror(errno));

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
        err = -1;
        hdlCloseErr(dir_name, 0);
    }

    return err;
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
