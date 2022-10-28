#include <dirent.h>
#include <errno.h>
#include <limits.h>  //PATH_MAX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  //snprintf
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include "util.h"
#include "optprsr.h"
#include "copy.h"

int main(int argc, char* argv[]) {
    struct stat infos;
    int err = stat(&infos, argv[1]);
    int serr = errno;
    printf("err: %d\n, infos:%p, %s\n", err, &infos, strerror(serr));

    return 0;
}
