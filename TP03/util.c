/**
 * @file util.c
 * @brief Utility functions mostly used for error handling
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "util.h"

void* tryalc(void* allocReturn) {
    if (allocReturn) return allocReturn;
    //*malloc returned null => exiting with error message ENOMEM
    fprintf(stderr, "%s, Cannot allocate Memory\n", strerror(errno));
    exit(ENOMEM);
}