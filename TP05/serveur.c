// Standard libraries
#include <stdio.h>
#include <stdlib.h>

// Error 
#include <errno.h>

// Types
#include <sys/types.h>

// Sockets
#include <sys/socket.h>




int main(int argc, char* argv[]) {

    // The port is the first argument given when run
    int port = argv[1];

    // Check that the port number is between 1024 and 65535
    if (port < 1024 || port > 65535) {
        int errnum = errno;
        fprintf(stderr, "Port number is not between 1024 and 65535.\n", stderror(errnum));
        return -1;
    }

    // Modified code from lecture 8: FIFO

    // Create the address
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    // Use the port given
    address.sin_port = htons(port);

    // Create a server socket
    int serverSock = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSock == -1) {
        int errnum = errno;
        fprintf(stderr, "Could not create server socket.\n", stderror(errnum));
        return -1;
    }

    // Attaches socket to address
    int checkPort = bind(serverSock, (struct sockaddr *) &address, sizeof(address));

    // Check that the port isn't already being used
    if (checkPort == -1) {
        int errnum = errno;
        fprintf(stderr, "Port is already being used. Try another one.\n", stderror(errnum));
        return -1;
    }

    // Socket is marked as passive
    listen(sock, 1);

    // While the program is running, listen for new clients
    while(1) {
        struct sockaddr_in clientAddress;
        unsigned int clientLength = sizeof(clientAddress);
        int clientSock = accept(serverSock, (struct sockaddr *) &clientAddress, &clientLength);

        /* Lectures/Ecritures sur clientSock (read/write) */
        // Read/Write here

        close(clientSock);
    }

    close(serverSock);
}