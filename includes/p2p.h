#ifndef P2P_H
#define P2P_H

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define N 5 // max number of connections per device

typedef struct {
	// address of the sender
	struct in_addr local_addr;
	// connection port 
	unsigned short port;
} p2p_header;

/* structure maintains connection(s)
 * can act as client or server */
typedef struct {
	int nconn;
	struct sockaddr_in addr[N];
	int socket[N];
	int connection[N]; // for accepting connections (server)
} p2p_struct;

int accept_p2p(p2p_struct *conn, short int port, unsigned short n); // listens/accepts incoming connections
int connect_p2p(p2p_struct *session, short int port, unsigned short n); // searches for connections to create

p2p_struct *init_p2p();

void close_p2p(p2p_struct *session); // deallocates memory/closes sockets

void *listen_p2p(void *arg); // constantly listens for incoming connections
void *recieve_data(void *arg); // send data to connection
void *send_data(void *arg); // recieve data from connection

#endif
