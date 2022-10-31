#include <stdio.h>
#include <stdlib.h>
#include <string.h>     //strerror
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>   // open
#include <sys/types.h>  // idem
#include <fcntl.h>      //  idem
#include <unistd.h>     // close
#include <limits.h>
#include <time.h>

#include "copy.h"
#include "util.h"
#include "files.h"

int is_modified(const char* src, const char* dest) {
    if (!exists(dest)) return 1; //* If destination does not exists => src is obviously newer
    
    struct stat src_infos = stat_s(src), dest_infos = stat_s(dest);
    int cmprison = dateCmpr(src_infos.st_mtim, dest_infos.st_mtim);
    
    return cmprison > 0 || src_infos.st_size != dest_infos.st_size;
}


int create_subpaths_ifneeded(const char* from, const char* to, const char* dest) {
    //In copy folders dest has exist so we can resolve abs path of dest then append rest
    //printf("from: %s,\n to:%s,\n dest:%s\n\n", from, to, dest);
    if (!exists(dest)) {
        errno = ENOENT;
        fprintf(stderr, "Cannot copy %s to %s, Dest: %s: %s\n", from, to, dest, strerror(ENOENT));
        return -1;
    }

    const char* abs_dest = absPath(dest);  //abs_dest for sure exists => will try to locate folders from this path
    int abs_dest_len = strlen(abs_dest);

    // search where dest and to differ (we have that 'dest' is included in 'to')
    // if dest is not parent dir of 'to'
    const char* dest_name = getFileName(dest);
    //printf("destname: %s\n", dest_name);
    char* dest_in_to_idx = strstr(to, dest_name);
    if (dest_in_to_idx == NULL) {
        errno = EINVAL;
        fprintf(stderr, "Cannot copy %s : %s is not a parent of %s\n", from, dest, to);
        return -1;
    }
    //extract where dest and to differ
    char* path_after = &dest_in_to_idx[strlen(dest_name) + 1];
    //built absolute version of path 'to' even if it doesn't exists
    const char* path = calloc(PATH_MAX, sizeof(char));
    if (concat_path(path, abs_dest, path_after) < 0) {
        hdlCatErr(to);
        return -1;
    }

    int path_len = strlen(path);
    //printf("abs_dest: %s,\n path_after:%s,\n path:%s,\n", abs_dest, path_after, path);
    //printf("len:%d, Path: %s\n", path_len, path);

    int* sp_idxs = calloc(path_len, sizeof(int));  //indices of each end of subpath i.e. 4 and 8  for /foo/bar/to
    int sp_len = 0;

    for (int i = abs_dest_len + 1; i < path_len; i++) {
        if (path[i] == '/') {
            sp_idxs[sp_len++] = i;
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


int copy(const char* from, const char* to) {
    int fd_from, fd_to;
    char buff[4096];  // size of a block
    ssize_t readNb;   // number of bytes read

    // lstat the source file/folder
    struct stat infos = lstat_s(from);

    // Error handling if unable to open source in read-only or the destination in write-only, create or excl (error if create but file already exists)
    if ((fd_from = open(from, O_RDONLY)) < 0) return hdlOpenErr(from, 0);

    fd_to = open(to, O_WRONLY | O_CREAT , 0644);
    if (fd_to < 0) {
        hdlOpenErr(from, 0);
        return -1;
    }
    // Check that it's a regular file, folder or a link
    if (!(S_ISREG(infos.st_mode) || S_ISDIR(infos.st_mode) || S_ISLNK(infos.st_mode))) {
        errno = EINVAL;
        fprintf(stderr, "\"%s\": Wrong file type\n", from);
        return -1;
    }
    
    //* While there are bytes left to be read, reads them 4096 by 4096 (4096 or current size of 'buff' if its not that)
    //* nb: if readNb is < to what we expected we dont really care because the program will retry until having read everything
    while ((readNb = read(fd_from, buff, sizeof buff)) > 0) {
        char* crtToCopy = buff;  // pointer to start of current 'portion of data' to copy
        ssize_t writtenNb = 0;
        do {
            writtenNb = write(fd_to, buff, readNb);
            if (writtenNb < 0 && errno != EINTR) return hdlCopyErr(from, to, 0, 1, fd_from, fd_to);
            // If serious writing error (other than just the call getting interrupted) happened => stop copying and return -1

            readNb -= writtenNb;
            crtToCopy += writtenNb;  // advance buffer section to copy
        } while (readNb > 0);
    }
    if (readNb < 0) {
        hdlReadErr(from, 0, 0, (int)NULL);
        return hdlCopyErr(from, to, 1, 0, fd_from, fd_to);
    }
    if (close(fd_to) < 0) {
        close(fd_from);           // we already have a closing error that we do not now how to handle, 
        return hdlCloseErr(to, 0);// so if we have another we cannot do much more than what we're already doing
    }
    if (close(fd_from) < 0) return hdlCloseErr(from, 0);

    return EXIT_SUCCESS;
}


int copy_ifneeded(const char* from, const char* to) {
    if (is_modified(from, to)) {
        int out = copy(from, to);
        return out;
    }
    
    //else printf("%s -- up to date\n", to);
    return 0;
}

int ultra_cp_single(const char* from, const char* to, int modif_perm, int preserve_link) {
    int out = copy_ifneeded(from, to);
    if (modif_perm) chmod(to, 0777);
    if (copy_ifneeded(from , to) < 0) {
        int savedErr = errno;
        printf("Cannot ultra cp : %s\n", strerror(savedErr));
        return -1;
    }
    return out;
}

//return where backuped file should be in dest
const char* compute_backup_dest(const char* src_parent, const char* src_path, const char* dest){
    const char* parent_name = getFileName(src_parent);
    char* parent_in_src_idx = strstr(src_path, parent_name);
    //extract where dest and to differ
    char* path_after = &parent_in_src_idx[strlen(parent_name) + 1];
    //built absolute version of path 'to' even if it doesn't exists
    const char* path = calloc(PATH_MAX, sizeof(char));
    concat_path(path, dest, path_after);
    return path;
}

/**
 * Backup files from 'parent/' to 'dest'
 * @param parent parent directory of files to backup
 * @param dir_name current directory to backup
 * @return 0 if success else error
 */
int ultra_cp(const char* parent, const char *dir_name, const char* dest, int modif_perm, int preserve_link) {
    int err = errno;
    DIR *d = opendir(dir_name);
    struct dirent *entry;
    const char *d_name;  // name of entry

    // In case of exception on opening
    if (!d) return hdlOpenErr(dir_name, 0);

    int is_created_subfolder = 0;
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
            //printf(" %s %*ld      %s  %s\n", permissions, 13, infos.st_size, modif_time, d_name);
            //* computes the name of the subdirectory and checks if it is not too long
            if (concat_path(path, dir_name, d_name) < 0) return hdlCatErr(d_name);
            if (lstat(path, &infos) < 0) fprintf(stderr, "Cannot stat %s: %s\n", d_name, strerror(errno));

        }
        // Is 'entry' a subdirectory ?
        if (entry->d_type & DT_DIR) {
            if (!isEntDot) {
                //printf("\n");
                err = ultra_cp(parent, path, dest, modif_perm, preserve_link);
            }
        } else {
           // if entry is a file => copy it if needed 
           const char* backup_loc = compute_backup_dest(parent, path, dest);
           printf("Copying %s to %s.\n\n", path, backup_loc);
            if (!is_created_subfolder) {
                create_subpaths_ifneeded(path, backup_loc, dest);
                is_created_subfolder = 1;
            }
            if (ultra_cp_single(path, backup_loc, modif_perm, preserve_link) < 0) return -1;
            //ultra-cp single already handle error
        }
    }
    // closing directory
    if (closedir(d)) {
        err = -1;
        hdlCloseErr(dir_name, 0);
    }

    return err;
}

