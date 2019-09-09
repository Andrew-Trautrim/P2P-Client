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

int sconn; // # of server connections
int cconn; // # of client connections
char *local_ip; // local ip address

/* structure maintains connection,
 * can act as either client or server */
typedef struct {
	char ip[32]; // connection identification
	int active, connected, port;
	int socket, connection; // sockets for connecting/accepting
	struct sockaddr_in addr;
} p2p_struct;

p2p_struct **session; // holds all connections

int connect_p2p(int i); // searches for connections to create

p2p_struct *init_p2p(int port); // allocates memory and sets port

void close_p2p(p2p_struct **session); // deallocates memory/closes sockets
void *accept_p2p(void *arg); // accepts incoming connections

#endif
