#include <dirent.h>
#include <errno.h>
#include <limits.h>  //PATH_MAX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  //snprintf
#include <sys/stat.h>
#include <sys/types.h>

int computePerm();

void test() {
    DIR *d = opendir(".");
    struct dirent *entry;
    const char *d_name;  // name of entry


    while ((entry = readdir(d)) != NULL) {
        struct stat infos;
        d_name = entry->d_name;
        stat(d_name, &infos);
        //int mode = infos.st_mode;
        computePerm(infos.st_mode, d_name);

        //printf("%s\n", d_name);
        //printf("mode:f %d\n", mode);
        //if (strcmp(d_name, "a.out") == 0) computePerm(mode);
        //printf("----------------\n\n");
    }
}

char permRepr[] = {'r', 'w', 'x'};

int computePerm(int mode, char* name) {
    //* 3 groups: owner, group, others, 3 perm: read write execute
    int x = 01, r = 04, w = 02;
    int permNb = 3, groupNb = 3;  //
    int perms[] = {r, w, x};
    char meta[10];
    for (int i = 0; i < 10; i++) meta[i] = '-';
    
    for (int i = 2; i >= 0; i--) {
        //printf("i = %d\n", i);
        //printf("-");
        for (int j = 0; j < permNb; j++) {
            
            int crtPerm = mode & (perms[j] << i * permNb);
            if (crtPerm) printf("%c", permRepr[j]);
            else printf("%c", '-');
            //printf("perm: %c \n", permRepr[j]);
        }
        
        /* int crtReadPerm = (mode & (r << i * permNb)) > 0;
        int crtWritePerm = (mode & (w << i * permNb)) > 0;
        int crtExecPerm = (mode & (x << i * permNb)) > 0; */

       /*  printf("read perm: %c \n", crtReadPerm);
        printf("write perm: %d \n", crtWritePerm);
        printf("execute perm: %d \n", crtExecPerm); */
    }
    printf("  %s\n\n", name);

    return 0;
}

int main() {
    test();

    return 0;
}
