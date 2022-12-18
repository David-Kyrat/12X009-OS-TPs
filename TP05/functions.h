#ifndef __FUNCTIONS__
#define __FUNCTIONS__

#define AF (AF_INET)

#define SOCK_TYPE SOCK_STREAM

#define PROTOCOL 0

#define EOT (";##;") // End Of Transmission
#define EOTlen (4)

typedef struct sockaddr_in sockaddr_in;

int new_socket();

struct hostent* resolve_hostname(const char* ip_addr);


sockaddr_in new_sockaddr(int port, struct hostent* server);


int init_serv_and_wait_con(int port, int max_pending_con_nb, int* bind_sockfd_toFill, int* con_sockfd_toFill, sockaddr_in* serv_addr_toFill, sockaddr_in* client_addr_toFill);

int init_serv(int port, int max_pending_con_nb, int* bind_sockfd_toFill, int* con_sockfd_toFill, sockaddr_in* serv_addr_toFill);


int serv_wait_con(int port, int bind_sockfd, int* con_sockfd_toFill, sockaddr_in* client_addr_toFill);


int init_client_and_connect(const char* ip_addr, int port, int* sockfd);

/**
 * Based on last message returned by read on socket => determine if there
 * is still some data left to be read or not (just compare last 4 bytes
 * with EOT)
 */ 
int sock_hasNext(const char* msg);


int sock_write(int sockfd, const char* msg);


char* read_all_data_from_socket(int sockfd, int buf_size) ;

#endif // __FUNCTIONS__

