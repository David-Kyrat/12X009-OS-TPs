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

static const char* USAGE_MSG_CLIENT = "Usage: %s <ip-address> (in decimal)  <portNumber> (2Bytes integer in [1024, 65535]) \n";

int main(int argc, char* argv[]) {
    /* int port = argv[2];ip
    const char* ip_addr = argv[1]; */
    int port;
    const char* ip_addr;
    int err_code = parseArgvClient(argc, argv, &port, &ip_addr);
    if (err_code != 0) return EXIT_FAILURE;

    sockaddr_in address = new_sockaddr(port, ip_addr);

    // Create a client socket
    int client_socket = new_socket();
    if (client_socket < 0) return -1;

    // Attempt at establishing connection
    if (connect(client_socket, (struct sockaddr*)&address, sizeof(address)) < 0) return -1;

    // Find out what the interval is
    int min, max;

    read(client_socket, &min, 4);
    printf("Minimum: %d\n", min);

    read(client_socket, &max, 4);
    printf("Maximum: %d\n", max);

    int received = -1;

    while (received != 0) {
        // Each time a proposition is sent to the server
        printf("Guess the number: ");
        int proposition;
        scanf("%d", &proposition);
        write(client_socket, &proposition, 4);
        printf("Proposition sent: %d\n\n", proposition);

        // Each time an answer is received from the server
        read(client_socket, &received, 4);

        switch (received) {
            // If the number was guessed correctly
            case 0:
                printf("Number guessed correctly, you win!\n");
                received = 0;
                close(client_socket);
                exit(EXIT_SUCCESS);

            // If the number we proposed was too low
            case 1:
                printf("Number is higher\n");
                break;

            // If the number we proposed was too high    
            case 2:
                printf("Number is lower\n");
                break;

            // If the number we proposed was too high
            case 4:
                printf("Too many guesses, you lose!\n");
                received = 0;
                close(client_socket);
                exit(EXIT_SUCCESS);

            default:
                errno = EINVAL;
                fprintf(stderr, "Unexpected value: %d. \n", received);
                exit(EINVAL);
        }
       /*  if (received == 0) {
        }

        else if (received == 1) {
        }

        // If the number we proposed was too high
        else if (received == 2)

            if (received == 4) {
                printf("Too many guesses, you lose!\n");
                received = 0;
                close(client_socket);
                
                break;
            } */
    }
}
