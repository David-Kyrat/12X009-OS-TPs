
# Implementation Plan

## Objective

- Implement a server that receives a client connection from any interface and shows its socket and IP address. This can be tested with *telnet ip_serveur port_serveur*
- Implement a client that connects to a server, then closes its socket and stops.
- Exchange information between the client and server (Random number generation, sending, receving etc.)
- The server should accept multiple connection simultaneously.
- Check that the client sockets are closed with *ss -tn*

---

## Client

---

### What to do:

Client takes 2 parameters: the IP address to server, and the port to connect to.

Example:

    ./client 127.0.0.1 65100

- The client will receive the interval chosen by the server in which the client can pick a number.
- The client gives a number until the server tells the client that they have won or they have lost (after a certain number of tries, also decided by the server).
- Display "Proposition envoyée: *proposition*" each time a message is **sent** to the server.
- Display "La valeur réelle est: *valeur*" each time a messages is **received** from the server.

---

### How to do it:

1. Create socket
2. Connect a socket to server
3. Read/Write from socket
4. Close socket

---

### Functions:

```c 
    int socket(int domain, int type, int protocol);                                     -> creates a socket
    int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);            -> initiates a connection
```

---

### Example of TCP client:

```c
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
```

---

## Server

---

### What to do:

Server takes 1 parameter: The port number it will be waiting at

Example:

    ./serveur 65100

The port number must be between **1024** and **65535** and shouldn't be used by another server.

The server is in charge of:
- accepting client connections
- generating a random number for each client (number should be in the same interval as all clients)
- respond to each client proposition accordingly
- manage several clients at a time

The random number should be optained by reading the file **/dev/urandom** which generates random numbers each time it's read.

The server should display:
-  "Client *socket* connecté avec l’ip *ip*" when connecting a new client.
-  "La valeur *valeur* est choisie pour le client *socket*" when a new random number is chosen for the client.
-  "Client *socket* propose *proposition*" when a client proposition is received.
---

### How to do it:

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

```c
    int socket(int domain, int type, int protocol);                                   -> creates a socket
    int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);             -> attaches socket to address
    int listen(int sockfd, int backlog);                                              -> marks a socket as passive (can accept connections)
    int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);                -> accepts a connection
```

---

### Example of TCP server:

```c
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
```
