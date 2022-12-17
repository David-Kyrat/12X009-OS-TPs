/* TCP client */

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "functions.h"
#include "util.h"

/*int init_and_connect(const char* ip_addr, int port, int* sockfd) {
    int _sockfd;
    struct hostent* server = resolve_hostname(ip_addr); 
    if ((_sockfd = new_socket()) < 0) return EXIT_FAILURE;
    
    struct sockaddr_in address = new_sockaddr(port, server);
    printf("trying to connect to host: %s, port: %d\n", ip_addr, port);
    if (connect(_sockfd, (struct sockaddr *)&address, sizeof(address)) < 0)
      printRErr("connect: %s, host: %s, port: %d\n", ip_addr, port);  

    *sockfd = _sockfd;

    return EXIT_SUCCESS;
}*/


int write_inp_to_sock(int sockfd, char* buffer) {
    memzero(buffer, 256);
    fgets(buffer, 255, stdin);

    if (write(sockfd, buffer, strlen(buffer)) < 0) 
      printRErr("write_inp_to_sock: %s, msg: %s, socket_fd:%d\n", buffer, sockfd);
    return EXIT_SUCCESS;
}


int read_from_sock(int sockfd, char* buffer) {
    memzero(buffer, 256);
    if (read(sockfd, buffer, 255) < 0) 
      printRErr("read_from_sock: %s, msg: %s, socket_fd:%d\n", buffer, sockfd);
    return EXIT_SUCCESS;
}


int main(int argc, char *argv[]) {
    if (argc < 3) {
        errno = EINVAL;
        printErr("%s.\nUsage: %s <hostname> <port-number>\n", argv[0]);
        return EXIT_FAILURE;
    }
    const char* ip_addr = argv[1]; int port = atoi(argv[2]), sockfd;
    //if ((sockfd = new_socket()) < 0) return EXIT_FAILURE;
    if ( init_client_and_connect(ip_addr, port, &sockfd) < 0) return EXIT_FAILURE;
    
    char* buffer = tryalc(malloc(256));
    printf("Please enter the message: ");
    write_inp_to_sock(sockfd, buffer);
    buffer = read_all_data_from_socket(sockfd, 0);

    printf("%s\n", buffer);
    close(sockfd);
    return 0;
}

