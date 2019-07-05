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

/* structure maintains connection,
 * can act as either client or server */
typedef struct {
	char *ip; // connection identification
	int socket, connection; // sockets for connecting/accepting
	unsigned short port;
	struct sockaddr_in addr;
} p2p_struct;

int connect_p2p(p2p_struct *session); // searches for connections to create
int send_data(p2p_struct *session);

p2p_struct *init_p2p(unsigned short port); // allocates memory and sets port

void close_p2p(p2p_struct *session); // deallocates memory/closes sockets

void *accept_p2p(void *arg); // accepts incoming connections
void *read_client(void *arg); // reads data sent from clients
void *read_server(void *arg); // reads data sent from the server connection

#endif
