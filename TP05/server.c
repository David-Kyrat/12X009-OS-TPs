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
// open
#include <fcntl.h>
// close
#include <sys/wait.h>
#include <unistd.h>

#include "functions.h"
#include "optprsr.h"
#include "util.h"


const int INTERVAL_MIN = 0;
const int INTERVAL_MAX = 64;  
const int GUESSES = 5;

const char* USAGE_MSG_SERV = "Usage: %s <portNumber> (2Bytes integer in [1024, 65535]) \n";
const int MAX_PEND_CNCTN = 5;  //* Maximum length to which the queue of pending connections for sockfd may grow.

int main(int argc, char* argv[]) {

    // Check the port
    int port = atoi(argv[1]);
    if (!isPortValid(port)) {
        printf("Invalid port\n");
        return -1;
    }


    sockaddr_in address = new_sockaddr(port, NULL);

    // Create a server socket
    int server_socket = new_socket();
    if (server_socket < 0) return -1;

    // Attaches socket to address
    int checkPort = bind(server_socket, (struct sockaddr*)&address, sizeof(address));

    // Check that the port isn't already being used or any other error
    if (checkPort == -1) {
        printRErr("%s, port %d : Cannot bind/assign an address to the server socket.\n", port);
        return -1;
    }
    // Socket is marked as passive
    listen(server_socket, MAX_PEND_CNCTN);

    for (;;) {  // While the program is running, listen for new clients
        sockaddr_in clientAddress;
        uint clientLength = sizeof(clientAddress);
        printf("Waiting for clients at port %d.\n", port);

        int client_socket = accept(server_socket, (struct sockaddr*)&clientAddress, &clientLength);

        // Code taken from slides

        // Make a temporary fork
        pid_t t_pid = fork();
        printf("PID: %d\n", t_pid);

        if (t_pid < 0) {
            printErr("%s: %s - Cannot Fork.\n", argv[0]);
            exit(EXIT_FAILURE);
        }

        if (t_pid > 0) {
            waitpid(t_pid, NULL, 0);
        }

        if (t_pid == 0) {
            pid_t pid = fork();

            // If we're on the parent processus, handle the client
            if (pid == 0) {
                const char* pretty_clientAddress = inet_tostr(&clientAddress.sin_addr);

                // Display when a new client is connected
                printf("Client %d connected with IP: %s.\n", client_socket, pretty_clientAddress);

                // Read / Write here
                srand(time(NULL));
                int numberToGuess = (rand() % 65);
                
                // dev/urandom doesnt work well on WSL, so a simple random number is used, but the code of dev/urandom is provided below
                //
                // FILE *f;
                // f = fopen("/dev/urandom", "r");
                // fread(&numberToGuess, sizeof(int), 1, f);  
                // fclose(f);


                // Display when a new random number is chosen for the client
                printf("Selected value for client %d: %d.\n", client_socket, numberToGuess);

                // Communicate the interval to the client
                write(client_socket, &INTERVAL_MIN, 4);
                write(client_socket, &INTERVAL_MAX, 4);

                int proposition;
                int result;
                int guess = 1;

                // Check for the results given back by the client
                while (proposition != numberToGuess && guess <= GUESSES) {
                    read(client_socket, &proposition, 4);
                    printf("Client %d proposes: %d\n", client_socket, proposition);

                    if (proposition == numberToGuess) {
                        result = 0;
                        write(client_socket, &result, 4);
                        break;
                    }

                    else if (proposition < numberToGuess) {
                        result = 1;
                        write(client_socket, &result, 4);
                        guess++;
                    }

                    else if (proposition > numberToGuess) {
                        result = 2;
                        write(client_socket, &result, 4);
                        guess++;
                    }
                }

                if (guess > GUESSES) {
                    result = 4;
                    write(client_socket, &result, 4);
                }

                free(pretty_clientAddress);
                close(client_socket);
            }
        }

    }

    close(server_socket);
}
