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

const int INTERVAL_MIN = 0; // to be replaced
const int INTERVAL_MAX = 64; // to be replaced
const int GUESSES = 5;

const char* USAGE_MSG_SERV = "Usage: %s <portNumber> (2Bytes integer in [1024, 65535]) \n";
const int MAX_PEND_CNCTN = 1; //* Maximum length to which the queue of pending connections for sockfd may grow.

int main(int argc, char* argv[]) {

    int port;
    // Parse arguments and check that the port number is between 1024 and 65535
    if (parseArgvServ(argc, argv, &port) < 0) printRErr("%s\n  %s", USAGE_MSG_SERV);

    sockaddr_in address = new_sockaddr(port, NULL);

    // Create a server socket
    int server_socket = new_socket();
    if (server_socket < 0) return -1;

    // Attaches socket to address
    int checkPort = bind(server_socket, (struct sockaddr *) &address, sizeof(address));

    // Check that the port isn't already being used or any other error
    if (checkPort == -1) printRErr("%s, port %d : Cannot bind/assign an address to the server socket.\n", port);

    // Socket is marked as passive
    listen(server_socket, MAX_PEND_CNCTN);
    
    for(;;) { // While the program is running, listen for new clients
        sockaddr_in clientAddress; uint clientLength = sizeof(clientAddress);
        int client_socket = accept(server_socket, (struct sockaddr *) &clientAddress, &clientLength);

        const char* pretty_clientAddress = inet_tostr(&clientAddress.sin_addr);
        
        // Display when a new client is connected
        printf("Client %d connected with IP: %s.\n", client_socket, pretty_clientAddress);

        // Read / Write here
        int numberToGuess = 0; // replace with whatever is read in /dev/urandom

        // Display when a new random number is chosen for the client
        printf("Selected value for client %d: %d.\n", client_socket, numberToGuess);

        // Communicate the interval to the client
        write(client_socket, &INTERVAL_MIN, 4);
        write(client_socket, &INTERVAL_MAX, 4);
        //TODO: Check if works with only 1Byte. Apprently there are errors when sending 1Byte or less

        int proposition;
        int guess = 0;

        while (proposition != numberToGuess && guess <= GUESSES) {
            read(client_socket, &proposition, 4);
            printf("Client %d proposes: %d\n", client_socket, proposition);

            if (proposition == numberToGuess) {
                write(client_socket, 0, 100);
            }

            else if (proposition < numberToGuess) {
                write(client_socket, 1, 100);
                guess++;
            }

            else if (proposition > numberToGuess) {
                write(client_socket, 2, 100);
                guess++;
            }
            
        }



        free(pretty_clientAddress);
        close(client_socket);
    }

    close(server_socket);
}