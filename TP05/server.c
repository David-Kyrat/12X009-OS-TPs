/* TCP server */

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <wait.h>

#include "functions.h"
#include "util.h"

#define errprinta(mess, args...) fprintf(stderr, mess, args);

#define errprint(mess) fprintf(stderr, mess);

int initSigHandlers();

void clean_before_exit();

const int SPEAK_TIME = 5;  //? amount of times each client is allowed to speak. i.e. number of read/write exchange before cutting connection with client
                           // each read/write back and forth will be called a 'round'
const int MAX_CLI = 5;     // Max nb of client to have at the same time

int crt_cli_nb = 0, bind_sockfd;

struct pair {
    pid_t id;        // pid of child handling a specific client
    int con_sockfd;  // file descriptor of the socket connect the server to said client
};
typedef struct pair pair;
const pair NULLP = {-2, -2};  // null pair since we cant assign one to NULL

/*pair new_pair(pid_t id, int con_sockfd) {
    pair el = {
        .id = id,
        .con_sockfd = con_sockfd};

    pair t = {id, con_sockfd}
    return el;
}*/
pair clients[5];
void init_client(pair* cli, pid_t pid, int fd) {
    cli->id = pid;
    cli->con_sockfd = fd;
}
void _init_clients() {
    for (int i = 0; i < MAX_CLI; i++) {
        clients[i] = NULLP;
    }
}

// Return the fd of the socket used by child of pid 'pid' to talk with the client
int get_client(pid_t pid) {
    for (int i = 0; i < MAX_CLI; i++) {
        if (clients[i].id == pid) return clients[i].con_sockfd;
    }

    errno = EINVAL;
    printErr("%s: client for pid %d, not found.\n", pid);
    return -2;  // I think -1 is already a valid value used for something
}

// same as get_client but set element at its index in 'clients' to NULL
int pop_client(pid_t pid) {
    for (int i = 0; i < MAX_CLI; i++) {
        if (clients[i].id == pid) {
            int consock_fd = clients[i].con_sockfd;
            clients[i] = NULLP;
            return consock_fd;
        }
    }
    errno = EINVAL;
    printErr("%s: client for pid %d, not found.\n", pid);
    return -2;
}

int isNull(int idx) { return clients[idx].id == NULLP.id && clients[idx].con_sockfd == NULLP.con_sockfd; }

int add_client(pid_t pid, int con_sockfd) {
    // search for a free spot in 'clients'
    for (int i = 0; i < MAX_CLI; i++) {
        if (isNull(i)) {
            init_client(&clients[i], pid, con_sockfd);
            return EXIT_SUCCESS;
        }
    }
    return EXIT_FAILURE;
}

int close_client(pid_t pid) {
    int fd = pop_client(pid);
    if (fd == -2) return -1;
    close(fd);
    return EXIT_SUCCESS;
}

/** kill signal to all children: 0 for sigint, 1 for sighup, 2 for sigterm 3 for sigkill. (default is SIGHUP) */
void killAllChildren(int sig) {
    errprinta("(%d) called killAllChildren with sig %d\n", getpid(), sig); 
    int signum;
    switch (sig) {
        case 0:
            signum = SIGINT;
            break;
        case 1: 
            signum = SIGHUP;
            break;
        case 2:
            signum = SIGTERM;
            break;
        case 3:
            signum = SIGKILL;
            break;
        default:
            signum = SIGHUP;
            break;
    }

    for (int i = 0; i < MAX_CLI; i++) {
        if (!isNull(i)) {
            if (kill(clients[i].id, signum) < 0) printErr("killAllChildren : %s, pid: %d\n", clients[i].id);
            clients[i] = NULLP;
        }
    
    }
}


void printSt() {
    errprint("------- Clients --------\n");
    errprint("idx\t pid\t sock_fd\n");
    for (int i = 0; i < MAX_CLI; i++) {
        if (isNull(i)) {
            errprinta("%d\t NULL\n", i);
        } else {
            errprinta("%d\t %d\t   %d\n", i, clients[i].id, clients[i].con_sockfd);
        }
    }
    errprint("\n");
}

// Takes a ".text string" and return malloc'ed address pointing to initialized variable
char* new_str(char* value) {
    if (value == NULL) return NULL;
    size_t len = strlen(value);
    char* out = strndup(value, len);  // let strndup add the "\0" at the end of out (regardless of whether it was present or not in 'value')
    return out;
}

void clean_before_exit() {
    errprinta("(%d) called before exit.\n", getpid());
    for (int i = 0; i < MAX_CLI; i++) {
        if (!isNull(i)) {
            if (close(clients[i].con_sockfd) < 0)
                printErr("%s: Could not close client-connection socket %d, pid:%d\n", clients[i].con_sockfd, clients[i].id); 
 
            clients[i] = NULLP;
        }
    }
    if (close(bind_sockfd) < 0){
        printErr("%s: Could not close bind_sockfd, fd: %d\n", bind_sockfd);
    } 
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        errno = EINVAL;
        printErr("%s.\nUsage: %s <port-number>\n", argv[0]);
        return EXIT_FAILURE;
    }
    _init_clients();
    initSigHandlers();

    int con_sockfd, portno = atoi(argv[1]);
    struct sockaddr_in serv_addr, cli_addr;
    
    char buffer[256];

    atexit(clean_before_exit);
    //void kac_wrapper() { killAllChildren(0); }
    //atexit(kac_wrapper); //! Child will inherits the atexit function and kill other child => extremely dangerous

    if (init_serv(portno, 5, &bind_sockfd, &con_sockfd, &serv_addr) < 0)
        return EXIT_FAILURE;

    memzero(buffer, 256);

    while (1) {
        if (serv_wait_con(portno, bind_sockfd, &con_sockfd, &cli_addr) < 0) {
            errprint("\n");
            continue;
        }
        crt_cli_nb++;
        const char* pretty_addr = inet_ntoa(cli_addr.sin_addr);
        printf("Client connected at IP: %s\n\n", pretty_addr);
        pid_t child_pid = fork();
        int child_exit_status;
        if (child_pid < 0) perror("accepting connection, cannot fork");
        if (child_pid == 0) {
            /* IN CHILD */

            // while client has not reached 5 rounds
            for (int round = 0; round < SPEAK_TIME; round++) {
                char* buf = sock_read(con_sockfd, 0);
                if (buf == NULL || strlen(buf) <= 0) continue;  // if we read nothing just wait for next mesasage

                printf("Here is the message from (%d, %d): \"%s\"\n", getpid(), con_sockfd, buf);

                char* msg = new_str("I got your message, it was: ");
                msg = realloc(msg, strlen(msg) + strlen(buf) + 1);
                strncat(msg, buf, strlen(buf));

                sock_write(con_sockfd, msg);
            }  // at the end of the 'SPEAK_TIME' rounds => close connection
               // TOFIX: Parent still has a fd open to socket in table des canneaux because we only closed it in child

            errprinta("Client at adress %s, spoke for %d round or Interrupted Connection. Closing socket...\n\n", pretty_addr, MAX_CLI);
            close(con_sockfd);
            exit(EXIT_SUCCESS);  // Exit from child

        } else if (child_pid > 0) {
            add_client(child_pid, con_sockfd);
            printSt();
            errprint("\n");
            // If too much client are talking => wait for 1 to finish and terminate associated child process. ELse just wait for new connections
            if (crt_cli_nb >= MAX_CLI) {
                int err_code = wait_s(&child_exit_status);
                crt_cli_nb = (err_code == 2) ? 0 : crt_cli_nb - 1;
                if (err_code > 0) errprinta("Child exited with exit code %d\n", child_exit_status);
            }
        }
    }

    close(bind_sockfd);

    return EXIT_SUCCESS;
}

/** ==================== SIGNALS =========================== */

/**
 * Signal Handler => Call the signal handling function corresponding to the given signal.
 *
 * @param sig signal number
 * @param info pointer to a siginfo_t structure
 * @param sh pointer shell instance
 */
void manage_signals(int sig, siginfo_t* info, void* ucontext) {
    switch (sig) {
        case SIGCHLD: {
            pid_t child = info->si_pid;
            int exit_stat;
            waitpid_s(child, &exit_stat);
            close_client(child);
            errprinta("closed client %d\n\n", child);
            printSt();
            errprint("\n");
        } break;

        case SIGINT:
            puts("sigint");
            //killAllChildren(0);
            exit(0); // default handler sigint doesnt seem to call function registered with atexit().
        default:
            break;
    }
}

int initSigHandlers() {
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO | SA_RESTART;

    printf("Pid: %d\n", getpid());
    sa.sa_sigaction = manage_signals;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGCHLD);
    sigaddset(&sa.sa_mask, SIGINT);
    /*for (int i = 0; i < SIG_NB; i++)
        sigaddset(&sa.sa_mask, SIG_TO_HDL[i]);*/

    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        hdlSigHdlErr("SIGCHLD", 0);
        return -1;
    }
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        hdlSigHdlErr("SIGCHLD", 0);
        return -1;
    }

    //
    //* Handle Signals to handle
    //
    /*for (int i = 0; i < SIG_NB; i++) {
        if (sigaction(SIG_TO_HDL[i], &sa, NULL) == -1) {
            const char msg[15];
            sprintf(msg, "SIG_TO_HDL[%d]\n", i);
            hdlSigHdlErr(msg, 1);
        }
    }*/
    //
    //* Ignore Signals to ignore
    //
    /*struct sigaction sa_ign;  // use same sa struct does not work

    sigemptyset(&sa_ign.sa_mask);
    for (int i = 0; i < SIG_NB; i++) sigaddset(&sa_ign.sa_mask, SIG_TO_IGNORE[i]);

    sa_ign.sa_handler = SIG_IGN;
    sa_ign.sa_flags = SA_RESTART;
    for (int i = 0; i < IGNORE_NB; i++) {
        if (sigaction(SIG_TO_IGNORE[i], &sa_ign, NULL) == -1) {
            const char msg[20];
            sprintf(msg, "SIG_TO_IGNORE[%d]\n", i);
            hdlSigHdlErr(msg, 0);
        }
    }*/

    return EXIT_SUCCESS;
}
