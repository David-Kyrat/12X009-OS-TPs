

/**
 * Receives the output of the "malloc attempt". If it's null, print an error message to stderr and exit
 * @param allocReturn The return value of malloc.
 * @return allocReturn if it's not null, exit otherwise
 */
void* tryalc(void* allocReturn);