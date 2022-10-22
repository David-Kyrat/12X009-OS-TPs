#include <dirent.h>
#include <errno.h>
#include <limits.h>  //PATH_MAX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  //snprintf
#include <sys/types.h>

/** @return Is 'entry_name' '.' or '..' ? */
int isDot(const char *entry_name) {
    return strcmp(entry_name, "..") == 0 || strcmp(entry_name, ".") == 0;
}

static void list_dir(const char *dir_name) {
    
    //TODO: CHECK IF IS FILE, BASE ON INODE INFO
    
    DIR *d = opendir(dir_name);
    struct dirent *entry;
    const char *d_name;  // name of entry

    // In case of exception on openning
    if (!d) {
        fprintf(stderr, "Cannot open directory '%s': %s\n", dir_name, strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("%s/:\n", dir_name);

    
    // Loop on each entry
    while ((entry = readdir(d)) != NULL) {
        // Get entry name and displays it
        d_name = entry->d_name;
        int isEntDot = isDot(d_name); //* is 'entry' '..' or '.' ?
        if (!isEntDot) printf("--- %s/%s\n", dir_name, d_name); //* Do not print ., .. as they are always here

        // Is 'entry' a subdirectory ?
        if (entry->d_type & DT_DIR) {
            if (!isEntDot) { 
                char path[PATH_MAX];

                // computes the name of the subdirectory and checks if it is too long
                if (snprintf(path, PATH_MAX, "%s/%s", dir_name, d_name) >= PATH_MAX) {
                    fprintf(stderr, "Path length has gotten too long.\n"); // Check that subdir pathname isnt too long
                    exit(EXIT_FAILURE);
                }               

                // recursive call & print carriage return to help cleanly separate directories and subdirectories
                printf("\n");
                list_dir(path);
            }
        }
    }

    // closing directory
    if (closedir(d)) {
        fprintf(stderr, "Could not close '%s': %s\n", dir_name, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    //list_dir("/var/log/");

    if (argc <= 1) {
        fprintf(stderr, "%s: Not Enough arguments: Expecting at least 1\n", argv[0]);
        return EXIT_FAILURE;
    }

    list_dir(argv[1]);

    return EXIT_SUCCESS;
}