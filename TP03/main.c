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

/**
 * BIT Field for the different behaviors (states) the program must take according to how it was called
 * and what / how many arguments were given. (i.e. called with 2 files, mulitple folders, just 1 file...)
 */
//* ST for 'STATE'
#define ST_JUST_LIST (1 << 0)      //! 000001 = 1

//? only 2 files are given as argument both exists
#define ST_2FILES  (1 << 1)        //! 000010 = 2

//? only 2 files and destination does not exist
#define ST_2FILES_CREATE (1 << 2)  //! 000100 = 4

//? multiple folders
#define ST_MULT_FOLDER (1 << 3)    //! 001000 = 8

//? -a was passed Links are copied as Links and resolved to ensure that both points to the same inode 
#define ST_PRESERVE_LINKS (1 << 4) //! 010000 = 16

//? -f was passed. Permissions of files in destination are changed even if the weren't replaced
#define ST_MODIF_PERM (1 << 5)     //! 100000 = 32



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
    
    // Set state as 0
    int state = 0;
    
    // If there is only one argument, simply print the contents of the directory
    if (fileNb <= 1) return state;


    // Destination file/folder is the last one
    const char* dest = files[fileNb - 1];
    
    // If there are only 2 files
    if (fileNb <= 2 && isFile(files[0], 0)) state += 1;

    switch (optional_state) {
        case 3: state += 2; // If both -f and -a are passed 
        break;
        case 1: state += 4; // If -f is passed
        break;
        case 2: state += 8; // If -a is passed
        break;
    }

    return state;
   /*  // If both -f and -a are passed
    if (optional_state == 3) {
            state += 2;
    }

    // If -f is passed
    else if (optional_state == 1) {
            state += 4;
    }

    // If -a is passed
    else if (optional_state == 2) {
            state += 8;
    } */


}

    // Check for
    // DONE -- 1. Just 1 file/folder -> print/ls contents
    // DONE -- 2. If only 2 files are given -> create/replace the file
    // TODO -- 3. If multiple files & folder and 'dest' exists -> create/replace architecture in 'dest'
    // TODO -- 4. If -a is passed, change the permissions
    // TODO -- 5. If -f is passed, links are copied as links (stored in optional state)

int ultra_cp(char* src, char* dest, int a, int f) {

    struct stat infos = lstat_s(src);

    // If the two files are exactly the same, do not copy
    if (is_modified(src, dest) == 0) return 0;
    
    // if -a was passed, change the permissions to 777 (rwx)
    if (a == 1) chmod(src, 777);

    // if -f was passed, copy from the realpath of the source (point to same inode as source link)
    if (f == 1) copy(realpath("src", 200), dest);

    if (S_ISDIR(infos.st_mode)) {
        list_dir(src, 1, dest);
    }

    else copy(src, dest);

    return 0;

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

    // Destination file/folder is the last one
    const char* dest = files[fileNb - 1];


    errno = 0;
    int state = handleArgs(fileNb, files, optional_state);

    // If there is only one argument, simply print the contents of the directory
    if (fileNb <= 1) return list_dir(files[0], 0, "");

    
    switch (state) {
        case 0:
            // Copy normally
            for (int i = 0; i < fileNb; i++) {
                ultra_cp(files[i], dest, 0, 0);
            }
            break;

        case 1:
            // Only 2 files were given
            ultra_cp(files[0], dest, 0, 0);
            break;

        case 2:
            // -f and -a were passed, multiple files
            for (int i = 0; i < fileNb; i++) {
                ultra_cp(files[i], dest, 1, 1);
            }
            break;

        case 3:
            // -f and -a were passed, only 2 files
            ultra_cp(files[0], dest, 1, 1);
            break;

         case 4:
            // -f passed, multiple files
            for (int i = 0; i < fileNb; i++) {
                ultra_cp(files[i], dest, 0, 1);
            }
            break;
        
         case 5:
            // -f passed, only 2 files
            ultra_cp(files[0], dest, 0, 1);
            break;

        case 8:
            // -a passed, multiple files
            for (int i = 0; i < fileNb; i++) {
                ultra_cp(files[i], dest, 1, 0);
            }
            break;
        
        case 9:
            // -a passed, only 2 files
            ultra_cp(files[0], dest, 1, 0);
            break;

        default:
            // return an error since none of these cases
            return -1;
            break;
    }

    for (int i = 0; i < fileNb; i++) {
        const char* file = files[i];
        struct stat infos;

        if (S_ISREG(infos.st_mode)) {
            infos = lstat_s(file);
            listEntry(file, infos);
            //TODO: if copy => handle file copying
    

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
}

// ------------------------------ OLD CODE ----------------------------------------

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



// -----------------------------------------------------------------------------------
