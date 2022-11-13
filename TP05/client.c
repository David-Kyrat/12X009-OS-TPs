// Standard libraries
#include <stdio.h>
#include <stdlib.h>

// Error 
#include <errno.h>



int main(int argc, char* argv[]) {
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    inet_pton(AF_INET, "192.168.1.1", &(address.sin_addr));
    address.sin_family = AF_INET;
    address.sin_port = htons(8080);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    connect(sock, (struct sockaddr *) &address, sizeof(address));
    ...
    read(sock, ...)
    write(sock, ...)
}