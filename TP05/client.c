// Standard libraries
#include <stdio.h>
#include <stdlib.h>
// memset
#include <string.h>
// basic string manip
#include <strings.h>
// Error
#include <errno.h>
// Types
#include <sys/types.h>
// Sockets, inet addresses
#include <arpa/inet.h>
#include <sys/socket.h>
//open
#include <fcntl.h>
//close
#include <unistd.h>

#include "functions.h"
#include "optprsr.h"
#include "util.h"

const char* USAGE_MSG_CLIENT = "Usage: %s <ip-address>(in decimal)  <portNumber> (2Bytes integer in [1024, 65535]) \n";

int main(int argc, char* argv[]) {
    
    int port;
    const char* ip_addr;
    if (parseArgvClient(argc, argv, &port, &ip_addr) < 0) printRErr("%s\n  %s", USAGE_MSG_CLIENT);

    sockaddr_in address = new_sockaddr(port, ip_addr);

    // Create a client socket
    int client_socket = new_socket();
    if (client_socket < 0) return -1;

    // Attempt at establishing connection
    if (connect(client_socket, (struct sockaddr *) &address, sizeof(address)) < 0) return hdlClientConnectErr(port, ip_addr);

    
        
    /* read(sock, ...)
    write(sock, ...) */
}