#include "functions.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "optprsr.h"
#include "util.h"


int isPortValid(int port) {
    if (port < MIN_PORT || port > MAX_PORT) {
        errno = EINVAL;
        printErr("%s, %d: Invalid port. Expected number in [1024, 65535]\n", port);
        return 0;
    }
    return 1;
}


/**
 * Return the conversion of an  IP address' string representation to binary.
 * @param addr_repr (Can Be Null) string representation of the IP address (or 'NULL' to default to 'INADDR_ANY', Usefull when create socket address for a server)
 * 
 * @return Initialized struct in_addr on success or nothing (exits with exit code EXIT_FAILURE) if an error happened.
 */
struct in_addr* new_in_addr(const char* addr_repr) {
    struct in_addr* inaddr = tryalc(malloc(sizeof(struct in_addr)));
    memset(inaddr, 0, sizeof(*inaddr));
    if (addr_repr == NULL) inaddr->s_addr = htonl(INADDR_ANY);

    else if (inet_pton(AF, addr_repr, inaddr) <= 0) {
        printErr("%s, %s: Cannot convert address to binary form.\n", addr_repr);
        exit(EXIT_FAILURE);
    }
    return inaddr;
}


sockaddr_in new_sockaddr(int port, const char* addr_repr) {
    // Create the address
    sockaddr_in address;

    //initialize it at 0
    memset(&address, 0, sizeof(address));
    
    struct in_addr* in_addr = new_in_addr(addr_repr);
    address.sin_addr = *in_addr;
    
    address.sin_family = AF;
    address.sin_port = htons(port);

    free(in_addr);

    return address;
}

int socket_predef() {
    return socket(AF, SOCK_TYPE, PROTOCOL);
}