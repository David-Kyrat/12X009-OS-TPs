#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>     //snprintf
#include <sys/stat.h>   //stat
#include <sys/types.h>  // stat
#include <time.h>
#include <unistd.h>  //  stat
#include <limits.h>

#include "copy.h"
#include "files.h"
#include "optprsr.h"
#include "util.h"

/**
 * BIT Field for the different behaviors (states) the program must take according to how it was called
 * and what / how many arguments were given. (i.e. called with 2 files, mulitple folders, just 1 file...)
 */
//* ST for 'STATE'
//! 000000 = 0
#define ST_JUST_LIST (0) //? Just list content of directory

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
#define ST_MIX (1 << 5)//? Mix of more than 1 folder and/or files (e.g. standard state where just copy entries from one directory to another)


int handleArgs(int fileNb, char** files, int optional_state) {
    // If there is only one argument, simply print the contents of the directory
    if (fileNb <= 1) return ST_JUST_LIST;

    // int returned by optional_state is coherent with the bitfield implemented here
    // i.e. we just have to add the value of optional_state to get the different case where -a, -f were passed or not.
    // so we initialize state as "0 + optional_state"
    int state = optional_state;

    // Destination file/folder is the last argument
    const char* dest = files[fileNb - 1];

    if (exists(dest)) {
        struct stat dest_infos = stat_s(dest);
        struct stat f0_infos = stat_s(files[0]);
        int isF0File = S_ISREG(f0_infos.st_mode), isDestFile = S_ISREG(dest_infos.st_mode),
            isF0Dir = S_ISDIR(f0_infos.st_mode), isDestDir = S_ISDIR(dest_infos.st_mode);

        // If there are only 2 file/folder as argument, emulate behavior of 'cp file1 file2' / 'cp file1 folder2'
        if (fileNb <= 2) {
            if (isF0File && (isDestDir || isDestFile))
                state += isDestFile ? ST_2FILES : ST_1FILE_1DIR;

            else if (isF0Dir && isDestDir)
                state += ST_MIX;

            //* Unauthorized state
            else {
                errno = EINVAL;
                return -1;
            }
        } else {
            if (isDestFile) {
                fprintf(stderr, "Cannot copy multiple files/folders onto the same destination file \"%s\".\n", dest);
                errno = EINVAL;
                return -1;
            }

            state += ST_MIX;
        }

    } else {  //If destination does not exists
        //if there is only 2 files on entry, emulate behavior of 'cp file1 file2'
        if (fileNb <= 2 && isFile(files[0], 0))
            state += ST_2FILES;

        else {  //if user entered multiple files and folder
            errno = ENOENT;
            return -1;  //Directory does not exists => "throws" no such file or directory error
        }
    }

    return state;
}


// Check for
    // 1. Just 1 file/folder -> print/ls contents
    // 2. If only 2 files are given -> create/replace the file
    // 3. If multiple files & folder and 'dest' exists -> create/replace architecture in 'dest'
    // 4. If -a is passed, change the permissions
    // 5. If -f is passed, links are copied as links (stored in optional state)

int main(int argc, char* argv[]) {
    //list_dir("/var/log/");
    int fileNb = -1, err = 0;

    // Parse the given arguments
    int optional_state = parseOptArgs(argc, argv);
    const char** files = parseArgs(argc, argv, &fileNb);

    // Check if parsing had any error
    if (optional_state < 0 || !files || fileNb <= 0) {
        int errno_cpy = errno;
        fprintf(stderr, "Cannot parse arguments: %s\n\t - Usage: %s folder1 folder2/ destination \n\n", strerror(errno_cpy), argv[0]);
        return EXIT_FAILURE;
    }
    // Destination file/folder is the last one
    const char* dest = files[fileNb - 1];

    errno = 0;
    int state = handleArgs(fileNb, files, optional_state);
    if (state < 0) {
        int savedErr = errno;
        fprintf(stderr, "%s, Cannot Backup to %s: %s\n", argv[0], dest, strerror(savedErr));
        return EXIT_FAILURE;
    }
    int modif_perm = state & ST_MODIF_PERM, preserve_links = state & ST_PRESERVE_LINKS;
    switch (state) {
        case ST_JUST_LIST:
            for (int i = 0; i < fileNb; i++) {
                if (!isDir(files[i]))
                    listEntryNoIn(files[i]);
                else
                    list_dir(files[i]);
                printf("-----\n");
            }
            break;

        //* Only 2 files were given
        case ST_2FILES:
        case ST_2FILES | ST_PRESERVE_LINKS:
        case ST_2FILES | ST_MODIF_PERM:
        case ST_2FILES | ST_PRESERVE_LINKS | ST_MODIF_PERM: {
            char* src = absPath(files[0]);
            if (src == NULL) return EXIT_FAILURE;
            ultra_cp_single(src, dest, modif_perm, preserve_links);
            break;
        }

        //* Only 1 file and a folder were given
        case ST_1FILE_1DIR:
        case ST_1FILE_1DIR | ST_PRESERVE_LINKS:
        case ST_1FILE_1DIR | ST_MODIF_PERM:
        case ST_1FILE_1DIR | ST_PRESERVE_LINKS | ST_MODIF_PERM: {
            char path[PATH_MAX];
            const char* src = getFileName(files[0]);
            if (src == NULL) return EXIT_FAILURE;
            concat_path(path, dest, src);
            ultra_cp_single(files[0], path, modif_perm, preserve_links);
            break;
        }

        case ST_MIX:
        case ST_MIX | ST_PRESERVE_LINKS:
        case ST_MIX | ST_MODIF_PERM:
        case ST_MIX | ST_PRESERVE_LINKS | ST_MODIF_PERM: {
            const char* _dest = absPath(dest);
            //for each file as argument
            for (int i = 0; i < fileNb - 1; i++) {
                
                //* if its not a directory file just copy it 
                const char* file = absPath(files[i]);
                if (!isDir(file)) {
                    char* backup_dest = calloc((2*strlen(dest)+ strlen(file))+1, sizeof(char));
                    concat_path(backup_dest, dest, getFileName(file));
                    ultra_cp_single(file, backup_dest, modif_perm, preserve_links);
                    free(backup_dest);
                }
                //* if not recurisvely copy each one
                else ultra_cp(file, file, _dest, modif_perm, preserve_links);
            }
            printf("\n --- Backed up files ---\n\n");
            list_dir(_dest);
            break;
        }
    }

    return EXIT_SUCCESS;
}