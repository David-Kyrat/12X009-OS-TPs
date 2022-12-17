#ifndef __FUNCTIONS__
#define __FUNCTIONS__

#define AF (AF_INET)
#define SOCK_TYPE SOCK_STREAM
#define PROTOCOL 0
typedef struct sockaddr_in sockaddr_in;

int new_socket();

struct hostent* resolve_hostname(const char* ip_addr);


sockaddr_in new_sockaddr(int port, struct hostent* server);


int init_serv_and_wait_con(int port, int max_pending_con_nb, int* bind_sockfd_toFill, int* con_sockfd_toFill, sockaddr_in* serv_addr_toFill, sockaddr_in* client_addr_toFill);

#endif // __FUNCTIONS__

