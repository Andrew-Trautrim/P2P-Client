#ifndef P2P_H
#define P2P_H

#include <arpa/inet.h>
#include <errno.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

// max number of connections
int nconn;

/* structure maintains connection,
 * can act as either client or server */
typedef struct {
	char ip[32]; // connection identification
	int active;
	int socket, connection; // sockets for connecting/accepting
	unsigned short port;
	struct sockaddr_in addr;
} p2p_struct;

typedef struct {
	p2p_struct **server;
	p2p_struct *client;
} p2p_conn;

int connect_p2p(p2p_struct *session); // searches for connections to create

p2p_struct *init_p2p(unsigned short port); // allocates memory and sets port

void close_p2p(p2p_struct *session); // deallocates memory/closes sockets
void *accept_p2p(void *arg); // accepts incoming connections

#endif
