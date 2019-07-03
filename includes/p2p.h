#ifndef P2P_H
#define P2P_H

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define PORT 8080

typedef struct {
	// address of the sender
	struct in_addr local_addr;
	// connection port 
	unsigned short port;
} p2p_header;

/* structure that maintains connetion(s) */
typedef struct {
	struct sockaddr_in client_addr;
	int client_socket;
	struct sockaddr_in server_addr;
	int server_socket;
	int connection;
} p2p_struct;

int accept_p2p(p2p_struct *conn, short int port); // listens/accepts incoming connections
int connect_p2p(p2p_struct *session, short int port); // searches for connections to create
int transfer_data(p2p_struct *session); // send/revcieve data once connection is established

void close_p2p(p2p_struct *session); // deallocates memory/closes sockets
void delay(int ticks);

#endif
