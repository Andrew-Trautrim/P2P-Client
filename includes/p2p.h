#ifndef P2P_H
#define P2P_H

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define LISTEN 1

typedef struct {
	struct sockaddr_in client_addr;
	int client_socket;
	struct sockaddr_in server_addr;
	int server_socket;
	int conn;
} p2p_struct;

int accept_p2p(p2p_struct *session, int port);
int connect_p2p(p2p_struct *session, int port, char* ip);
int init_p2p_session(p2p_struct *session); // initiates sockets and main functionality

void close_p2p(p2p_struct *session); // deallocated memory
void init_p2p(p2p_struct *session); // allocates memory for p2p session

#endif
