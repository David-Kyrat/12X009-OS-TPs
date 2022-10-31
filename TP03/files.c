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
    /* struct stat infos;
    if (lstat(path, &infos) < 0) fprintf(stderr, "Cannot stat %s: %s\n", path, strerror(errno)); */
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

int create_subpaths_ifneeded(const char* from, const char* to, const char* dest) {
    //In copy folders dest has exist so we can resolve abs path of dest then append rest
    //printf("from: %s,\n to:%s,\n dest:%s\n\n", from, to, dest);
    if (!exists(dest)) {
        errno = ENOENT;
        fprintf(stderr, "Cannot copy %s to %s, Dest: %s: %s\n", from, to, dest, strerror(ENOENT));
        return -1;
    }

    int destlen = strlen(dest);
    //int path_after_len = strlen(to) - destlen;
    char* abs_dest = absPath(dest);  //abs_dest for sure exists => will try to locate folders from this path
    int abs_dest_len = strlen(abs_dest);

    // search where dest and to differ (we have that 'dest' is included in 'to')
    // if dest is not parent dir of 'to'
    char* dest_name = getFileName(dest);
    printf("destname: %s\n", dest_name);
    char* dest_in_to_idx = strstr(to, dest_name);
    if (dest_in_to_idx == NULL) {
        errno = EINVAL;
        fprintf(stderr, "Cannot copy %s : %s is not a parent of %s\n", from, dest, to);
        return -1;
    }
    //extract where dest and to differ
    char* path_after = &dest_in_to_idx[strlen(dest_name) + 1];
    //printf("dest:%s, destlen: %d,  to:%s, strlen(to): %d, strlen(path_after): %d \n", dest, destlen, to, strlen(to), path_after_len);

    const char* path = calloc(PATH_MAX, sizeof(char));
    if (concat_path(path, abs_dest, path_after) < 0) {
        hdlCatErr(to);
        return -1;
    }

    int path_len = strlen(path);
    printf("abs_dest: %s,\n path_after:%s,\n path:%s,\n", abs_dest, path_after, path);
    //printf("len:%d, Path: %s\n", path_len, path);
    //return 0;
    int* sp_idxs = calloc(path_len, sizeof(int));  //indices of each end of subpath i.e. 4 and 8  for /foo/bar/to
    int sp_len = 0;

    for (int i = abs_dest_len + 1; i < path_len; i++) {
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
        char* crt_subpath = strsub(path, sp_idxs[i]);
        if (!exists(crt_subpath)) {
            struct stat info = stat_s(from);
            if (mkdir(crt_subpath, 0700) < 0) {
                int savedErr = errno;
                fprintf(stderr, "Cannot create %s: %s\n", crt_subpath, strerror(savedErr));
            } else
                printf("-- Created %s -- \n\n", crt_subpath);
        }
        //printf("crt_subpath: %s\n", crt_subpath);
    }

    return EXIT_SUCCESS;
}

//OLD VERSION
int __list_dir(const char *dir_name, int determine_copy, char* copy_to_dest) {
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

        //TODO: figure why that was here
       /*  if (exists(entry) == -1) {
            FILE* f;
            
        } */

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

            if (determine_copy == 1) {
                copy(d_name, copy_to_dest);
            }

            //* computes the name of the subdirectory and checks if it is not too long
            if (concat_path(path, dir_name, d_name) < 0) return hdlCatErr(d_name);
            if (lstat(path, &infos) < 0) fprintf(stderr, "Cannot stat %s: %s\n", d_name, strerror(errno));

            // Is 'entry' a subdirectory ?
            if (entry->d_type & DT_DIR) {
                printf("\n");
                // err = list_dir(path);
                printf("\n%s/ (rest):\n", dir_name);
            }
        }

        // Is 'entry' a subdirectory ?
       /*  if (entry->d_type & DT_DIR) {
            if (!isEntDot) {
                printf("\n");
                err = list_dir(path);
                printf("\n%s/ (rest):\n", dir_name);
            }
        } */
    }
    // closing directory
    if (closedir(d)) {
        fprintf(stderr, "Could not close '%s': %s\n", dir_name, strerror(errno));
        exit(EXIT_FAILURE);
    }

    return err;
}