#include <sys/socket.h>
#include <arpa/inet.h>

#define AF (AF_INET) //set "global" (i.e. variable used everywhere) address domain to internet
#define SOCK_TYPE SOCK_STREAM // Use connection oriented socket-type (SOCK_STREAM)
#define PROTOCOL 0  // Use and specifically TCP (PROTOCOL = 0) (or UDP if SOCK_TYPE was set to using connectionless transmissions)

//* Structure describing an Internet socket address.
typedef struct sockaddr_in sockaddr_in;


/**
 * Checks if the given port number is valid (i.e. between optprsr.MIN_PORT and optprsr.MAX_PORT, (inclusive))
 * if not set errno and print error message to stderr.
 *
 * @param port The port number to check.
 *
 * @return 0 if port is invalid 1 if it is. (Returned values are such that they can be directly passed into an if statement. )
 */
int isPortValid(int port);



/**
 * Creates a socket address for a client or a server
 * 
 * @param port the port number to bind the socket to
 * @param addr_repr (Can Be Null) string representation of the IP address (or 'NULL' to default to 'INADDR_ANY', if creating socket address for the server side)
 * 
 * @return Initialized sockaddr_in struct just left to call the 'socket()' method on it to initiate connection (client) / finish binding-listen-accept (server).
 */
sockaddr_in new_sockaddr(int port, const char* addr_repr);
