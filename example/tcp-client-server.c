//!
//! ======================= TCP CLIENT ==============================
//!

struct sockaddr_in address;
memset( &address, 0, sizeof(address) );
inet_pton( AF_INET, "192.168.1.1", &(address.sin_addr) );
address.sin_family = AF_INET;
address.sin_port = htons(8080);

int sock = socket(AF_INET, SOCK_STREAM, 0);
connect(sock, (struct sockaddr *) &address, sizeof(address));
...
read(sock, ...)
write(sock, ...)


//!
//! ======================= TCP SERVER ==============================
//!

struct sockaddr_in address;
memset(&address, 0, sizeof(address));
address.sin_family = AF_INET;
address.sin_addr.s_addr = htonl(INADDR_ANY);
address.sin_port = htons(8080);

int serverSock = socket(AF_INET, SOCK_STREAM, 0);
bind( serverSock, (struct sockaddr *) &address, sizeof(address) );
listen(sock, 5);

while( 1 ) {
    struct sockaddr_in clientAddress;
    unsigned int clientLength = sizeof(clientAddress);
    int clientSock = accept(serverSock,
                            (struct sockaddr *) &clientAddress,
                            &clientLength);

    /* Lectures/Ecritures sure clientSock (read/write) */

    close( clientSock );
}

