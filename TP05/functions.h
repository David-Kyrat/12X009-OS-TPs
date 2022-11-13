/* Internet address struct. containing an IP address (uint32) */
typedef struct in_addr in_addr;


/**
 * Checks if the given port number is valid (i.e. between optprsr.MIN_PORT and optprsr.MAX_PORT, (inclusive))
 * if not set errno and print error message to stderr.
 * @param port The port number to check.
 * @return 0 if port is invalid 1 if it is. (Returned values are such that they can be directly passed into an if statement. )
 */
int isPortValid(int port);
