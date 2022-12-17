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

char* tread(int sockfd) {
    // Buffer index
    long index = 0, buf_size = 1024;
    long s = buf_size;
    char* buffer = tryalc(malloc(buf_size));

    // Bytes read by the socket in one go
    ssize_t bytesRead;

    while (1) {  // break condition specified on the basis of bytes read
        bytesRead = read(sockfd, buffer + index, buf_size - index);

        if (bytesRead <= 0) {
            // No more bytes to read from the socket, terminate the loop
            break;
        }
        // bytesRead has the number of bytes that have been already read,
        // Use it to increment the buffer index.
        index += bytesRead;

        /*if (index >= s) {
            s *= 2;
            //buffer = realloc(buffer, s);
        }*/
    }

    return buffer;
}

char* rread(int socket) {
    int s = 1024;
    char buffer[1024];

    // Buffer index
    int index = 0;

    // Bytes read by the socket in one go
    ssize_t bytesRead;

    while (1)  // break condition specified on the basis of bytes read
    {
        bytesRead = read(socket, buffer + index, sizeof(buffer) - index);

        if (bytesRead <= 0) {
            // No more bytes to read from the socket, terminate the loop
            break;
        }
        printf("sizeof(buf): %ld, bytesRead: %ld, tot: %d, buf: %s\n", sizeof(buffer), bytesRead, index, buffer);

        // bytesRead has the number of bytes that have been already read,
        // Use it to increment the buffer index.
        index += bytesRead;
    }

    return buffer;
}

int sread(int socket, char buffer[], int size) {
    
    
    // Buffer index
    int index = 0;

    // Bytes read by the socket in one go
    ssize_t bytesRead;

    while (1)  // break condition specified on the basis of bytes read
    {
        bytesRead = read(socket, buffer + index, size - index);

        if (bytesRead <= 0) {
            // No more bytes to read from the socket, terminate the loop
            break;
        }

        printf("bytesRead: %ld, tot: %d, buf: %s\n", bytesRead, index, buffer);
        // bytesRead has the number of bytes that have been already read,
        // Use it to increment the buffer index.
        index += bytesRead;
        break;
    }
    return bytesRead;
}

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

int main(int argc, char* argv[]) {
    if (argc < 3) {
        errno = EINVAL;
        printErr("%s.\nUsage: %s <hostname> <port-number>\n", argv[0]);
        return EXIT_FAILURE;
    }
    const char* ip_addr = argv[1];
    int port = atoi(argv[2]), sockfd;
    // if ((sockfd = new_socket()) < 0) return EXIT_FAILURE;
    if (init_client_and_connect(ip_addr, port, &sockfd) < 0) return EXIT_FAILURE;

    char* buffer = tryalc(malloc(256));
    printf("Please enter the message: ");
    write_inp_to_sock(sockfd, buffer);
    char buf[10001];
    int r = sread(sockfd, buf, 10000);
    printf("read %d bytes\n", r);
    printf("%s\n", buf);
    close(sockfd);
    return 0;
}
