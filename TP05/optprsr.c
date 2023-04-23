#include <errno.h>
#include <getopt.h>
#include <limits.h>  //PATH_MAX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "functions.h"
#include "optprsr.h"
#include "util.h"

#define USAGE_MSG_SERV "Usage: %s <portNumber> (2Bytes integer in [1024, 65535]) \n"
#define USAGE_MSG_CLIENT "Usage: %s <ip-address>(in decimal)  <portNumber> (2Bytes integer in [1024, 65535]) \n"

//static const char HELP_CHAR = '?', QUIT_CHAR = 'Q';
//const char* OPT_STRING = "fa";
const int MIN_ARG_NB_SERV = 1, MAX_ARG_NB_SERV = 1;
const int MIN_ARG_NB_CLIENT = 2, MAX_ARG_NB_CLIENT = 2;
//static const char* USAGE_MSG_CLIENT = "Usage: %s <ip-address>(in decimal)  <portNumber> (2Bytes integer in [1024, 65535]) \n";

const int PORT_SIZE = 2;

//* Size of the file given when calling program
size_t FILE_SIZE;

/**
 * @brief Check if there is at least 'minArgNb' and at max 'maxArgNb' argument.
 * @param argc same as argc in main
 * @return 0 if success otherwise code 22 (Invalid argument)
 */
int checkArgsNb(int argc, const int minArgNb, const int maxArgNb) {
    int relevantNb = argc - 1;
    if (relevantNb < minArgNb || relevantNb > maxArgNb) {
        errno = EINVAL;
        return EINVAL;
    }
    return 0;
}

int checkArgsNbServ(int argc) {
    return checkArgsNb(argc, MIN_ARG_NB_SERV, MAX_ARG_NB_SERV);
}
int checkArgsNbClient(int argc) {
    return checkArgsNb(argc, MIN_ARG_NB_CLIENT, MAX_ARG_NB_CLIENT);
}

/**
 * Parses the command line arguments and stores the port number and the ip address (if client
 * called) into the given *_dest arguments.
 * 
 * @param argc the number of arguments passed to the program
 * @param argv the array of arguments passed to the program
 * @param isServ 1 if called for server, 0 if it's for client
 * @param port_dest the port number to be used for the connection
 * @param ip_addr_dest Pointer to the IP address of the server (if client) or NULL (if server)
 *                     NB: does a malloc. ip_addr_dest must not have any memory allocated before. Also remember to free.
 * 
 * @return error code. -1 if an error happened (errno set appropriately), 0 for success.
 */
int parseArgv(int argc, char* argv[], int isServ, int* port_dest, char** ip_addr_dest) {
    int checkArgsErr = isServ ? checkArgsNbServ(argc) : checkArgsNbClient(argc);
    if (checkArgsErr != 0) {
        if (isServ) fprintf(stderr, USAGE_MSG_SERV, argv[0]);
        else fprintf(stderr, USAGE_MSG_CLIENT, argv[0]);
        return -1;
    }

    int port_idx = isServ ? 1 : 2;  // position of port value (as a string) in argv[]
    char* port_str = strndup(argv[port_idx], strlen(argv[port_idx]));
    int port = atoi(port_str);
    if (!isPortValid(port)) return -1;
    *port_dest = port;

    // parsing ip address if client called (i.e. isServ == 0)
    if (!isServ) {
        *ip_addr_dest = strndup(argv[1], strlen(argv[1]));
        if (*ip_addr_dest == NULL) {
            printErr("%s, %s: Cannot copy IP address.\n", *ip_addr_dest);
            exit(EXIT_FAILURE);  //No memory Left => Force-exiting program.
        } else printf("IP address: %s\n", *ip_addr_dest);
    }
    printf("Port: %d\n", *port_dest);
    return 0;
}


int parseArgvServ(int argc, char* argv[], int* port_dest) { return parseArgv(argc, argv, 1, port_dest, NULL); }

int parseArgvClient(int argc, char* argv[], int* port_dest, char** ip_addr_dest) { return parseArgv(argc, argv, 0, port_dest, ip_addr_dest); }