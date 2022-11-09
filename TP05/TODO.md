
# TODO
---
---


## **Client**


1. Create socket
2. Connect a socket to server
3. Read/Write from socket
4. Close socket

---

### Functions:
    int socket(int domain, int type, int protocol);                                     -> creates a socket
    int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);            -> initiates a connection

---

### Example of TCP client:


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



---
---






## **Server**

1. Create server socket
2. Attach server socket to an address
3. Listens for start of connection
4. Accepts a connection and obtain client socket
5. Read/Write from client socket
6. Close client socket
7. Repeat step 4 if needed
8. Close server socket

---

### Functions:
    int socket(int domain, int type, int protocol);                                   -> creates a socket
    int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);             -> attaches socket to address
    int listen(int sockfd, int backlog);                                              -> marks a socket as passive (can accept connections)
    int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);                -> accepts a connection

---
### Example of TCP server:

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

---
---


## Useful functions

    ssize_t recv(int sockfd, void *buf, size_t len, int flags);                         -> instead of read (for sockets)
    ssize_t send(int sockfd, const void *buf, size_t len, int flags);                   -> instead of write (for sockets)
    ssize_t sendfile(int out_fd, int in_fd, off_t *offset, size_t count)                -> also instead of read/write

---

## Unix sockets


    #define UNIX_PATH_MAX    108
    struct sockaddr_un {
        sa_family_t sun_family;        /* Famille: AF_UNIX*/
        char sun_path[UNIX_PATH_MAX];  /* Chemin sur le systeme de fichiers */
    };

---
---

