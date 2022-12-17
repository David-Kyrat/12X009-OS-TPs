#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netdb.h> // gethostbyname
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "functions.h"
#include "util.h"


int new_socket() {
    int sockfd = socket(AF, SOCK_TYPE, PROTOCOL); 
    if (sockfd < 0) printRErr("%s, Cannot open given socket\n", " ");
     return sockfd;
}

//used only by client
struct hostent* resolve_hostname(const char* ip_addr) {
    struct hostent* server = gethostbyname(ip_addr);
    if (server == NULL)
        printErr("resolve_hostname: %s, ip: %s\n", ip_addr); 
    return server;
}


sockaddr_in new_sockaddr(int port, struct hostent* server) {
    struct sockaddr_in address;
    memset((char *)&address, 0, sizeof(address)); 
    address.sin_family = AF;

    memcpy((char *) &address.sin_addr.s_addr, (char *) server->h_addr,  server->h_length);

    address.sin_port = htons(port);

    return address;
}


int init_serv(int port, int max_pending_con_nb, int* bind_sockfd_toFill, int* con_sockfd_toFill, sockaddr_in* serv_addr_toFill)  {

    int sockfd = -2, con_sockfd = -2;
    if ((sockfd = new_socket()) < 0) return -1;

    serv_addr_toFill->sin_family = AF;
    serv_addr_toFill->sin_addr.s_addr = INADDR_ANY;
    serv_addr_toFill->sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)serv_addr_toFill, sizeof(*serv_addr_toFill)) < 0)
        printRErr("In bind of init_serv: %s, port: %d\n", port); 

    if (listen(sockfd, max_pending_con_nb) < 0) 
        printRErr("In listen of init_serv: %s, port: %d, sock_fd:%d\n", port, sockfd); //TODO: close socket and other cleanup before exiting
   
    *bind_sockfd_toFill = sockfd;
    *con_sockfd_toFill = con_sockfd;

    return EXIT_SUCCESS;

}

int init_serv_and_wait_con(int port, int max_pending_con_nb, int* bind_sockfd_toFill, int* con_sockfd_toFill, sockaddr_in* serv_addr_toFill, sockaddr_in* client_addr_toFill)  {
    /*int sockfd = -2, con_sockfd = -2;
    if ((sockfd = new_socket()) < 0) return -1;

    serv_addr_toFill->sin_family = AF;
    serv_addr_toFill->sin_addr.s_addr = INADDR_ANY;
    serv_addr_toFill->sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)serv_addr_toFill, sizeof(*serv_addr_toFill)) < 0)
        printRErr("In bind of init_serv: %s, port: %d\n", port); 

    if (listen(sockfd, max_pending_con_nb) < 0) 
        printRErr("In listen of init_serv: %s, port: %d, sock_fd:%d\n", port, sockfd); //TODO: close socket and other cleanup before exiting*/

    init_serv(port, max_pending_con_nb, bind_sockfd_toFill, con_sockfd_toFill, serv_addr_toFill);
    printf("Awaiting connection at port: %d\n", port);
   
    socklen_t clilen = sizeof(*client_addr_toFill);
    *con_sockfd_toFill = accept(*bind_sockfd_toFill, (struct sockaddr *)client_addr_toFill, &clilen);
    if (*con_sockfd_toFill < 0)
        printRErr("In accept of init_serv: %s, port: %d, bind_sock_fd:%d\n", port, *bind_sockfd_toFill); //TODO: same here 

    /**bind_sockfd_toFill = sockfd;
    *con_sockfd_toFill = con_sockfd;*/

    return EXIT_SUCCESS;
}

int serv_wait_con(int port, int bind_sockfd, int* con_sockfd_toFill, sockaddr_in* client_addr_toFill) {

    printf("Awaiting connection at port: %d\n", port);
   
    socklen_t clilen = sizeof(*client_addr_toFill);
    *con_sockfd_toFill = accept(bind_sockfd, (struct sockaddr *)client_addr_toFill, &clilen);
    if (*con_sockfd_toFill < 0)
        printRErr("In accept of init_serv: %s, port: %d, bind_sock_fd:%d\n", port, bind_sockfd); //TODO: same here 

    return EXIT_SUCCESS;
}

int init_client_and_connect(const char* ip_addr, int port, int* sockfd) {
    int _sockfd;
    struct hostent* server = resolve_hostname(ip_addr); 
    if ((_sockfd = new_socket()) < 0) return EXIT_FAILURE;
    
    struct sockaddr_in address = new_sockaddr(port, server);
    printf("trying to connect to host: %s, port: %d\n", ip_addr, port);
    if (connect(_sockfd, (struct sockaddr *)&address, sizeof(address)) < 0)
      printRErr("connect: %s, host: %s, port: %d\n", ip_addr, port);  

    *sockfd = _sockfd;

    return EXIT_SUCCESS;
}

#define MIN(a, b) (a < b ? a : b)

//if buf_size is 0 => while read until EOF
char* read_all_data_from_socket(int sockfd, int buf_size) {
    int total_bytes_read = 0, bytes_read = 0;
    buf_size = buf_size <= 1 ? 256 : buf_size;
    int init_buf_size = buf_size;

    char* buffer = tryalc(malloc(init_buf_size));

    printf("min: %d\n", MIN(init_buf_size, (buf_size - total_bytes_read)));
    printf("buf_size: %d, total_bytes_read:%d, init_buf_size:%d\n", buf_size, total_bytes_read, init_buf_size);
    int toread = MIN(init_buf_size, (buf_size - total_bytes_read));

    while ((bytes_read = read(sockfd, buffer + total_bytes_read, toread)) > 0) {
        total_bytes_read += bytes_read;
        printf("bytes_read: %d\n", bytes_read);
        printf("buf_size: %d, total_bytes_read:%d, init_buf_size:%d\n", buf_size, total_bytes_read, init_buf_size);

        while (total_bytes_read >= buf_size - 1) {
            // buffer is full, read the next chunk
            buf_size *= 2;
            buffer = realloc(buffer, buf_size);
        }
        toread = MIN(init_buf_size, (buf_size - total_bytes_read));
        printf("toread: %d\n\n", toread);

    }
    printf("bytes_read: %d, total_bytes_read: %d\n", bytes_read, total_bytes_read);
    if (bytes_read < 0) {
        printErr("read_all_data_from_socket: %s, buf_size: %d, init_buf_size %d\n", buf_size, init_buf_size);
        return buffer;
    }

    return buffer;
}



