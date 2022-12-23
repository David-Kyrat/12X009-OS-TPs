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

#define errprint(mess, args...) fprintf(stderr, mess, args);

int initSigHandlers();

void clean_exit();

const int SPEAK_TIME = 5;  //? amount of times each client is allowed to speak. i.e. number of read/write exchange before cutting connection with client
                           // each read/write back and forth will be called a 'round'
const int MAX_CLI = 5;     // Max nb of client to have at the same time

int crt_cli_nb = 0;

struct pair {
    pid_t id;        // pid of child handling a specific client
    int con_sockfd;  // file descriptor of the socket connect the server to said client
};
typedef struct pair pair;
const pair NULLP = {-2, -2};//null pair since we cant assign one to NULL

/*pair new_pair(pid_t id, int con_sockfd) {
    pair el = {
        .id = id,
        .con_sockfd = con_sockfd};

    pair t = {id, con_sockfd}
    return el;
}*/
pair clients[MAX_CLI];
void init_clients() { for (int i = 0; i < MAX_CLI; i++) { clients[i] = NULLP;  } }


// Return the fd of the socket used by child of pid 'pid' to talk with the client 
int get_client(pid_t pid) {
    for (int i = 0; i < MAX_CLI; i++) {
        if (clients[i].id == pid) return clients[i].con_sockfd;
    }

    errno = EINVAL;
    printErr("%s: client for pid %d, not found.\n", pid);
    return -2; // I think -1 is already a valid value used for something
}

//same as get_client but set element at its index in 'clients' to NULL
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
    //search for a free spot in 'clients'
    for (int i = 0; i < MAX_CLI; i++) {
        if (isNull(i)) {
            pair tmp = {pid, con_sockfd};
            clients[i] = tmp;
            return EXIT_SUCCESS;
        }
    }
    return EXIT_FAILURE;
}

// Takes a ".text string" and return malloc'ed address pointing to initialized variable
char* new_str(char* value) {
    if (value == NULL) return NULL;
    size_t len = strlen(value);
    char* out = strndup(value, len);  // let strndup add the "\0" at the end of out (regardless of whether it was present or not in 'value')
    return out;
}

char* new_str2(char* value, size_t size) {
    if (value == NULL || size <= 0) return NULL;
    size_t len = strlen(value);
    char* out = tryalc(malloc(size <= len ? len + 1 : size));
    memzero(out, size);
    strncat(out, value, len);
    return out;
}

int close_client(pid_t pid) {
    int fd = get_client(pid);
    if (fd == -2) return -1;
    close(fd);

}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        errno = EINVAL;
        printErr("%s.\nUsage: %s <port-number>\n", argv[0]);
        return EXIT_FAILURE;
    }
    initSigHandlers();

    int bind_sockfd, con_sockfd, portno = atoi(argv[1]);
    // socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;

    void clean_exit() {
        //! Ideally store pid and con_sockfd of all client, / children and close each connection
        //! kill / wait each children here
        close(con_sockfd);
        close(bind_sockfd);
    }
    atexit(clean_exit);

    if (init_serv(portno, 5, &bind_sockfd, &con_sockfd, &serv_addr) < 0)
        return EXIT_FAILURE;

    memzero(buffer, 256);

    while (1) {
        if (serv_wait_con(portno, bind_sockfd, &con_sockfd, &cli_addr) < 0) {
            fprintf(stderr, "\n");
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

                printf("Here is the message: %s\n", buf);

                char* msg = new_str("I got your message, it was: ");
                msg = realloc(msg, strlen(msg) + strlen(buf) + 1);
                strncat(msg, buf, strlen(buf));

                sock_write(con_sockfd, msg);
            }  // at the end of the 'SPEAK_TIME' rounds => close connection
               // TOFIX: Parent still has a fd open to socket in table des canneaux because we only closed it in child

            errprint("Client at adress %s, spoke for %d round. Closing connection...\n\n", pretty_addr, MAX_CLI);
            close(con_sockfd);
            exit(EXIT_SUCCESS);  // Exit from child

        } else if (child_pid > 0) {
            // If too much client are talking => wait for 1 to finish and terminate associated child process. ELse just wait for new connections
            if (crt_cli_nb >= MAX_CLI) {
                int err_code = wait_s(&child_exit_status);
                crt_cli_nb = (err_code == 2) ? 0 : crt_cli_nb - 1;
                if (err_code > 0) fprintf(stderr, "Child exited with exit code %d\n", child_exit_status);
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
        } break;

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
    /*for (int i = 0; i < SIG_NB; i++)
        sigaddset(&sa.sa_mask, SIG_TO_HDL[i]);*/
    
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
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
