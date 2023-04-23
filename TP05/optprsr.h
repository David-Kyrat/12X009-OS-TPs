#ifndef __OPTPRSR__
#define __OPTPRSR__
#define MIN_PORT 1024 // Lowest Port value
#define MAX_PORT 65535 //Highest Port value


//* Unsigned char i.e. (should be) exactly 1 Byte 
typedef unsigned char byte;


/**
 * @brief Check if there is at least 'MIN_ARG_NB_SERV' and at max 'MAX_ARG_NB_SERV' argument.
 * 
 * @param argc same as argc in main
 * 
 * @return 0 if success otherwise code 22 (Invalid argument)
 */
int checkArgsNbServ(int argc);


/**
 * @brief Check if there is at least 'MIN_ARG_NB_CLIENT' and at max 'MAX_ARG_NB_CLIENT' argument.
 * 
 * @param argc same as argc in main
 * 
 * @return 0 if success otherwise code 22 (Invalid argument)
 */
int checkArgsNbClient(int argc);


/**
 * Parses the command line arguments and stores the port number into the given *_dest arguments.
 * 
 * @param argc the number of arguments passed to the program
 * @param argv the array of arguments passed to the program
 * @param port_dest the port number to be used for the connection
 * 
 * @return error code. -1 if an error happened (errno set appropriately), 0 for success.
 */
int parseArgvServ(int argc, char* argv[], int* port_dest);

/**
 * Parses the command line arguments and stores the port number and the ip address into the given *_dest arguments.
 *
 * @param argc the number of arguments passed to the program
 * @param argv the array of arguments passed to the program
 * @param port_dest the port number to be used for the connection
 * @param ip_addr_dest Pointer to the IP address of the server.
 *                     NB: does a malloc. ip_addr_dest must not have any memory allocated before. Also remember to free.
 *
 * @return error code. -1 if an error happened (errno set appropriately), 0 for success.
 */
int parseArgvClient(int argc, char* argv[], int* port_dest, char** ip_addr_dest);

/**
 * Reads a line from stdin, and parses it into the given variables, then checks if the parsed values
 * are valid
 * 
 * @param INP_FORMAT The format string for the input.
 * @param cmd command to be executed (g, s, w)
 * @param ltype type of lock to be placed. (r, w, u)
 * @param start start of lock location 
 * @param length end of lock location
 * @param whence relatively to what are start and end computed ? (s, c, e)
 * 
 * @return 0 if success, -1 if error (errno is set accordingly). Or 2 if user entered an '?' to get help.
 */
int parseInput(const char* INP_FORMAT, char* cmd, char* ltype, long* start, long* length, char* whence);


#endif /* __OPTPRSR__ */
