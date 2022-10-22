#include <dirent.h>
#include <errno.h>
#include <limits.h>  //PATH_MAX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  //snprintf
#include <sys/types.h>

int isDot(const char *entry_name) {
    return strcmp(entry_name, "..") == 0 || strcmp(entry_name, ".") == 0;
}

static void list_dir(const char *dir_name) {
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
        if (!isDot(d_name)) printf("--- %s/%s\n", dir_name, d_name);

        // Is 'entry' a subdirectory ?
        if (entry->d_type & DT_DIR) {
            //if 'entry' isn't neither '..' nor '.'
            if (strcmp(d_name, "..") != 0 && strcmp(d_name, ".") != 0) {
                char path[PATH_MAX];

                // computes the name of the subdir and prints it
                int path_length = snprintf(path, PATH_MAX, "%s/%s", dir_name, d_name);
                //printf("%s\n", path);
                //printf("\n%s/:\n", path);
                printf("\n");

                // Check that subdir pathname isnt too long
                if (path_length >= PATH_MAX) {
                    fprintf(stderr, "Path length has got too long.\n");
                    exit(EXIT_FAILURE);
                }

                // recursive call
                list_dir(path);
            }
        }
    }  //while(1)

    // closing directory
    if (closedir(d)) {
        fprintf(stderr, "Could not close '%s': %s\n",
                dir_name, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    //list_dir("/var/log/");

    if (argc <= 1) {
        fprintf(stderr, "Not Enough arguments: Expecting at least 1\n");
        return EXIT_FAILURE;
    }

    list_dir(argv[1]);

    return EXIT_SUCCESS;
}