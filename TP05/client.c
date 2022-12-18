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


int write_inp_to_sock(int sockfd, char* buffer) {
    memzero(buffer, 256);
    fgets(buffer, 255, stdin);

    if (write(sockfd, buffer, strlen(buffer)) < 0)
        printRErr("write_inp_to_sock: %s, msg: %s, socket_fd:%d\n", buffer, sockfd);
    return EXIT_SUCCESS;
}

/*int read_from_sock(int sockfd, char* buffer) {
    memzero(buffer, 256);
    if (read(sockfd, buffer, 255) < 0)
        printRErr("read_from_sock: %s, msg: %s, socket_fd:%d\n", buffer, sockfd);
    return EXIT_SUCCESS;
}*/

int main(int argc, char* argv[]) {
    if (argc < 3) {
        errno = EINVAL;
        printErr("%s.\nUsage: %s <hostname> <port-number>\n", argv[0]);
        return EXIT_FAILURE;
    }
    const char* ip_addr = argv[1];
    int port = atoi(argv[2]), sockfd;
    if (init_client_and_connect(ip_addr, port, &sockfd) < 0) return EXIT_FAILURE;


    char* buffer = tryalc(malloc(4096));
    printf("Please enter the message: ");
    if (write_inp_to_sock(sockfd, buffer) < 0) return EXIT_FAILURE;
 

    buffer = read_all_data_from_socket(sockfd, 0);
    printf("%s\n", buffer);
    if (close(sockfd) < 0) 
        hdlCloseErr("socket", 1);

    return EXIT_SUCCESS;
}

