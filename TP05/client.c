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
    memzero(buffer, 4097);
    fgets(buffer, 4096, stdin);

    if (sock_write(sockfd, buffer) < 0)
    //if (write(sockfd, buffer, strlen(buffer)) < 0)
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


    size_t buf_size = 4096;
    char* buffer = tryalc(malloc(buf_size+1));
    memzero(buffer, buf_size+1);
    
    printf("Please enter the message: ");
    fgets(buffer, buf_size, stdin);
    if (sock_write(sockfd, buffer) < 0) return EXIT_FAILURE;


    buffer = sock_read(sockfd, 0);
    printf("%s\n", buffer);
    if (close(sockfd) < 0) 
        hdlCloseErr("socket", 1);

    return EXIT_SUCCESS;
}

