/* TCP server */

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "functions.h"
#include "util.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        errno = EINVAL;
        printErr("%s.\nUsage: %s <port-number>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int bind_sockfd, con_sockfd, portno = atoi(argv[1]), n;
    //socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    if ( init_serv(portno, 5, &bind_sockfd, &con_sockfd, &serv_addr) < 0 ) 
        return EXIT_FAILURE;

    memzero(buffer, 256);

    while (1) {
        if (serv_wait_con(portno, bind_sockfd, &con_sockfd, &cli_addr) < 0) { 
            fprintf(stderr, "\n");
            continue;
        }
        n = read(con_sockfd, buffer, 255);
        if (n < 0) printErr("%s ERROR reading from socket\n", " ");
    
        printf("Here is the message: %s\n", buffer);
  
        char tmp[] = "I got your message";
        char* msg = strndup(tmp, strlen(tmp));

        //n = write(con_sockfd, msg, 18);
        n = sock_write(con_sockfd, msg); 
        if (n < 0) printErr("%s ERROR reading from socket\n", " ");
    
    }
    close(con_sockfd);
    close(bind_sockfd);
    
    return 0;
}
