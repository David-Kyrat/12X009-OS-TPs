#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>     //snprintf
#include <sys/stat.h>   //stat
#include <sys/types.h>  // stat
#include <time.h>
#include <unistd.h>  //  stat

#include "copy.h"
#include "files.h"
#include "optprsr.h"
#include "util.h"

/* 
*  if filesNb <= 1: just list file
*  else if filesNb > 1
*       if destination exists and is a regular file:
*           if filesNb > 2 => throw error (no more than 1 source if dest is a file)
*           else backup normally files and folder to dest (handle case if there is a diff then copy if not print "is up to date")
*       else if dest doesnt exist or is a folder : backup normally 
*
*
*
*/

int handleArgs(int fileNb, char** files, int optional_state) {
    

    // TODO: Check for
    // 1. Just 1 file/folder -> print contents
    // 2. If only 2 files are given -> create/replace the file
    // 3. If -a is passed, change the permissions
    // 4. If -f is passed, links are copied as links (stored in optional state)
    
    // Set state as 0
    int state = 0;
    
    // If there is only one argument, simply print the contents of the directory
    if (fileNb <= 1) return state;


    // Destination file/folder is the last one
    const char* dest = files[fileNb - 1];
    
    // If there are only 2 files
    if (fileNb <= 2 && isFile(files[0], 0)) {
            state = 1;
    }

    int dest_exists = exists(dest);
    if (dest_exists < 0) return -1;

    if (dest_exists) {
        //* if destination is a regular file or link to regular file
        if (isFile(dest, 0)) {
            if (fileNb > 2) {
                errno = EINVAL;
                fprintf(stderr, "%s: Cannot copy multiple files onto the same destination file", strerror(errno));
                return -1;
            }

            // TEST: backup files[0] to dest if newer or size different or print "up to date" if not
            if (is_modified(files[0], dest) == 1) {
                //copy(files[0], dest);
                state = 2;
            }

            else {
                state = 3;
                printf("Up to date\n");
            }
        }

        //TODO: backup files to the 'dest' folder while checking which is newer

    } else {
        //* If program was called with only 2 files and the first is a regular file or link to regular file
        
        } else {
            state = 5;
            //TODO: create dest dir & backup normally files (no need to check if files are newer)
        }
    }
    //struct stat dest_infos = lstat_s(dest);

    return state;
}

int main(int argc, char* argv[]) {
    //list_dir("/var/log/");
    int fileNb = -1, err = 0;

    // Parse the given arguments
    char** files = parseArgs(argc, argv, &fileNb);
    int optional_state = parseOptArgs(argc, argv);

    // Check if parsing had any error
    if (optional_state < 0 || !files || fileNb <= 0) {
        int errno_cpy = errno;
        fprintf(stderr, "%s: %s\n\t - Usage: %s folder1 folder2/ destination \n\n", argv[0], strerror(errno_cpy), argv[0]);
        return errno_cpy;
    }



    errno = 0;
    int state = handleArgs(fileNb, files, optional_state);

    
    switch (state) {
        case 0:
            return listEntryNoIn(files[0]);
            break;

        case 1:

            break;

        case 2:

            break;

        default:
            break;
            // default statements
    }

    for (int i = 0; i < fileNb; i++) {
        const char* file = files[i];
        struct stat infos;

        if (S_ISREG(infos.st_mode)) {
            infos = lstat_s(file);
            listEntry(file, infos);
            //TODO: if copy => handle file copying

        } else {
            err = list_dir(file, copy);
        }

        /*  -- Done: add copy.c to makefile to be able to compile it.
            TODO: check if source file has been modified (is_modified, copy.c). If it has, copy it (copy, copy.c). If not, ignore it
            TODO: if -a has been passed, change all the permissions, even if the file has not been replaced
            TODO: if -f has been passed, links should be copied as links and destination link should point to the source link inode (use realpath)
            -- Done: implement in copy => copy only reg file, folder and links
            TODO: implement parsing of dest options -a -f
            TODO: put list_dir & list_entry etc... into another file called ultra-cp or smth else
        */

        //if (err != 0) fprintf(stderr, "Error at file %d, %s : %s\n", i, files[i], strerror(err));
        printf("    ____________________  \n\n\n");
    }

    return err;
}

// ------------------------------ OLD CODE ----------------------------------------

// Destination file/folder is the last one
// const char* dest = files[fileNb - 1];

// int dest_exists = exists(dest);
// if (dest_exists < 0) return -1;

// TEST: backup files[0] to dest if newer or size different or print "up to date" if not
// if (is_modified(files[0], dest) == 1) {
//    copy(files[0], dest);

// else {
//     printf("Up to date\n");
//     }

// if (dest_exists) {
//     //* if destination is a regular file or link to regular file
//     if (isFile(dest, 0)) {
//         if (fileNb > 2) {
//             errno = EINVAL;
//             fprintf(stderr, "%s: Cannot copy multiple files onto the same destination file", strerror(errno));
//             return -1;
//         }

//     }
// }

// If there is only one argument, simply print the contents of the directory
// if (fileNb <= 1) return listEntryNoIn(files[0]);

// -----------------------------------------------------------------------------------
