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
#define LISTEN 0

typedef struct {
	struct sockaddr_in client_addr;
	int client_socket;
	struct sockaddr_in server_addr;
	int server_socket;
	int connection;
} p2p_struct;

char *read_input(FILE *fstream, char *input); // dynamically allocates memory basd on input

int accept_p2p(p2p_struct *conn, int port); // listens/accepts incoming connections
int connect_p2p(p2p_struct *session, int port, char* ip); // searches for connections to create
int transfer_data(p2p_struct *session); // transfers data once connection is established
int recieve_data(p2p_struct *session); // recieve incoming information through connection

void close_p2p(p2p_struct *session); // deallocates memory/closes sockets

#endif
