#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "optprsr.h"
#include "util.h"

#include "functions.h"

// Max Length in character of an IP address "xxx.xxx.xxx.xxx\0" => 13chars.
const socklen_t MAX_IPADDR_LEN = 13;

int isPortValid(int port) {
    if (port < 1024 || port > 65535) {
        errno = EINVAL;
        printErr("%s, %d: Invalid port. Expected number in [1024, 65535]\n", port);
        return 0;
    }
    return 1;
}


/**
 * Return the conversion of an IP address' string representation to binary.
 * @param addr_repr (Can Be Null) string representation of the IP address (or 'NULL' to default to 'INADDR_ANY', Usefull when create socket address for a server)
 * 
 * @return Initialized struct in_addr on success or nothing (exits with exit code EXIT_FAILURE) if an error happened.
 */
struct in_addr* new_in_addr(const char* addr_repr) {
    struct in_addr* inaddr = tryalc(malloc(sizeof(struct in_addr)));
    memset(inaddr, 0, sizeof(*inaddr));
    if (addr_repr == NULL) inaddr->s_addr = htonl(INADDR_ANY);

    else if (inet_pton(AF, addr_repr, inaddr) <= 0) {
        printErr("%s, %d: Cannot convert address to binary form.\n", addr_repr);
        exit(EXIT_FAILURE);
    }
    return inaddr;
}


const char *inet_tostr(struct in_addr* in_addr) {
    socklen_t size = MAX_IPADDR_LEN;
    char* dst = tryalc(calloc(size, sizeof(char)));
    inet_ntop(AF, in_addr, dst, size);
    return dst;
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

int new_socket() {
    int sock = socket(AF, SOCK_TYPE, PROTOCOL);
    if (sock < 0) printRErr("%s, Cannot open given socket\n", "");
    return sock;
}

/**
 * This function handles the error that occurs when a client cannot connect to a server.
 * 
 * @param port The port number that the client is trying to connect to.
 * @param ip_addr The IP address of the server.
 */
int hdlClientConnectErr(int port, const char* ip_addr) {
    printRErr("%s, Cannot connect to server at address %s and port %d.\n", ip_addr, port);
}
