/* TCP server */

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "functions.h"
#ifndef UTIL
#include "util.h"
#define UTIL
#endif

int main(int argc, char *argv[]) {
    if (argc < 2) {
        errno = EINVAL;
        printErr("%s.\nUsage: %s <port-number>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int bind_sockfd, con_sockfd, portno = atoi(argv[1]);
    //socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
   
    int n;
    /*sockfd = new_socket();
    //sockfd = socket(AF_INET, SOCK_STREAM, 0);
    //if (sockfd < 0) error("ERROR opening socket");

    memzero((char *)&serv_addr, sizeof(serv_addr));
    
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
   
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    printf("Awaiting connection at port: %d\n", portno);
    con_sockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (con_sockfd < 0)
        error("ERROR on accept");*/
    if ( init_serv_and_wait_con(portno, 5, &bind_sockfd, &con_sockfd, &serv_addr, &cli_addr) < 0 ) 
        return EXIT_FAILURE;

    memzero(buffer, 256);
    n = read(con_sockfd, buffer, 255);
    if (n < 0) error("ERROR reading from socket");
    
    printf("Here is the message: %s\n", buffer);
   
    n = write(con_sockfd, "I got your message", 18);
    if (n < 0) error("ERROR writing to socket");
   
    close(con_sockfd);
    close(bind_sockfd);
    return 0;
}
