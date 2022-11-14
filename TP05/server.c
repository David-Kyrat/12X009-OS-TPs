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

#define INTERVAL_MIN (0) // to be replaced
#define INTERVAL_MAX (64) // to be replaced

const char* USAGE_MSG_SERV = "Usage: %s <portNumber> (2Bytes integer in [1024, 65535]) \n";
const int MAX_PEND_CNCTN = 1; //* Maximum length to which the queue of pending connections for sockfd may  grow.

int main(int argc, char* argv[]) {

    int port;
    // Parse arguments and check that the port number is between 1024 and 65535
    if (parseArgvServ(argc, argv, &port) < 0) 
        printRErr("%s\n  %s", USAGE_MSG_SERV);

    /* if (server_socket == -1) {
        int errnum = errno;
        //fprintf(stderr, "%s: Could not create server socket.\n", strerror(errnum)); 
    
        return -1;
    } */

    sockaddr_in address = new_sockaddr(port, NULL);

    // Create a server socket
    int server_socket = new_socket();
    if (server_socket < 0) return EXIT_FAILURE;

    // Attaches socket to address
    int checkPort = bind(server_socket, (struct sockaddr *) &address, sizeof(address));


    // Check that the port isn't already being used
    if (checkPort == -1) {
        printRErr("%s, port %d : Cannot bind/assign an address to the server socket.\n", port);
        //int errnum = errno;

        //fprintf(stderr, "%s: Port is already being used. Try another one.\n", strerror(errnum));
        // Not everytime, and even tho it is asked to check if port is already in use
        // there doesnt seem to be a way to do that since two processes can connect to 
        // the same port if the address is different. Hence, we only need the check for the address
        //return -1;
    }

    // Socket is marked as passive
    listen(server_socket, MAX_PEND_CNCTN);

    // While the program is running, listen for new clients
    while(1) {
        sockaddr_in clientAddress; uint clientLength = sizeof(clientAddress);
        int client_socket = accept(server_socket, (struct sockaddr *) &clientAddress, &clientLength);

        const char* pretty_clientAddress = inet_tostr(&clientAddress.sin_addr);
        // Display when a new client is connected
        printf("Client %d connected with IP: %s.\n", client_socket, pretty_clientAddress);

        // Read / Write here
        int numberToGuess = 0; // replace with whatever is read in /dev/urandom

        // Display when a new random number is chosen for the client
        printf("Selected value for client %d: %d.\n", numberToGuess, client_socket);

        // Communicate the interval to the client
        /* write(client_socket, &INTERVAL_MIN, 1);
        write(client_socket, &INTERVAL_MAX, 1); */


        free(pretty_clientAddress);
        close(client_socket);
    }

    close(server_socket);
}