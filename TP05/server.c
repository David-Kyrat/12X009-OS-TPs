// Standard libraries
#include <stdio.h>
#include <stdlib.h>

#include <string.h> //memset
#include <strings.h>

// Error 
#include <errno.h>
// Types
#include <sys/types.h>
// Sockets, inet addresses
#include <sys/socket.h>
#include <arpa/inet.h>

#define INTERVAL_MIN (0) // to be replaced
#define INTERVAL_MAX (64) // to be replaced

int main(int argc, char* argv[]) {

    // The port is the first argument given when run
    int port = argv[1];

    // Check that the port number is between 1024 and 65535
    if (port < 1024 || port > 65535) {
        int errnum = errno;
        fprintf(stderr, "Port number is not between 1024 and 65535.\n", strerror(errnum));
        return -1;
    }

    // Modified code from lecture 8: FIFO

    // Create the address
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address)); //initialize it at 0
    address.sin_family = AF_INET; //set address domain to internet
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    // Use the port given
    address.sin_port = htons(port);

    // Create a server socket
    int serverSock = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSock == -1) {
        int errnum = errno;
        //fprintf(stderr, "%s: Could not create server socket.\n", strerror(errnum)); 
        //* Not everytime, and even tho it is asked to check if port is already in use
        //* there doesnt seem to be a way to do that since two processes can connect to 
        //* the same port if the address is different. Hence, we only need the check for the address

        return -1;
    }

    // Attaches socket to address
    int checkPort = bind(serverSock, (struct sockaddr *) &address, sizeof(address));

    // Check that the port isn't already being used
    if (checkPort == -1) {
        int errnum = errno;
        fprintf(stderr, "%s: Port is already being used. Try another one.\n", strerror(errnum));
        return -1;
    }

    // Socket is marked as passive
    //listen(sock, 1);

    // While the program is running, listen for new clients
    while(1) {
        struct sockaddr_in clientAddress;
        unsigned int clientLength = sizeof(clientAddress);
        int clientSock = accept(serverSock, (struct sockaddr *) &clientAddress, &clientLength);

        // Display when a new client is connected
        printf("Client %d connecté avec l'ip %d. \n", clientSock, clientAddress);

        // Read / Write here
        int numberToGuess = 0; // replace with whatever is read in /dev/urandom

        // Display when a new random number is chosen for the client
        printf("La valeur %d est choisie pour le client %d. \n", numberToGuess, clientSock);

        // Communicate the interval to the client
        /* write(clientSock, &INTERVAL_MIN, 1);
        write(clientSock, &INTERVAL_MAX, 1); */



        close(clientSock);
    }

    close(serverSock);
}