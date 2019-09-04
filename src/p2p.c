#include "p2p.h"

/* function connects to an existing server node in p2p network (client side) */
void *connect_p2p(void *arg) {

	p2p_struct *client = (p2p_struct*)arg;

	// creates client side socket
	if ((client->socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "[!] Unable to create client side socket for %s - error %d\n", client->ip, errno);
		return NULL;
	}

	// connection settings
	client->addr.sin_family = AF_INET;
	client->addr.sin_port = htons(client->port);

	// client side connection
	int response = -1;
	for(int i = 0; i < 5 && response == -1; ++i) {
		response = connect(client->socket, (struct sockaddr*)&(client->addr), sizeof(client->addr));
		if (response != -1)
			break;
		sleep(2);
	}
	if (response == -1) {
		fprintf(stderr, "[!] Unable to connect to %s - error %d\n", client->ip, errno);
		return NULL;
	}

	fprintf(stdout, "Connected to %s on port %d\n", client->ip, client->port);
	client->connected = 1;
	return NULL;
}

/* allocates memory for the session and sets the port */
p2p_struct *init_p2p(unsigned short port) {
	p2p_struct *session = (p2p_struct*)calloc(1, sizeof(p2p_struct));
	strcpy(session->ip, "UNKNOWN");
	session->connection = 0;
	session->active = 0;
	session->connected = 0;
	session->port = port;
	return session;
}

/* deallocates memory and closes sockets*/
void close_p2p(p2p_struct **session) {
	for (int i = 0, n = cconn+sconn; i < n; ++i) {
		close(session[i]->socket);
		close(session[i]->connection);
		free(session[i]);
	}
	return;
}

/* function initiates network by listening for incoming connections (server side) */
void *accept_p2p(void *arg) {
	p2p_struct *server = (p2p_struct*)arg;

	// creates server side socket
	if ((server->socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "[!] Unable to create server side socket - error %d\n", errno);
		return NULL;
	}

	// connection settings
	server->addr.sin_family = AF_INET;
	server->addr.sin_port = htons(server->port);
	server->addr.sin_addr.s_addr = INADDR_ANY;

	// binds socket to port
	if (bind(server->socket, (struct sockaddr*)&(server->addr), sizeof(server->addr)) < 0) {
		fprintf(stderr, "[!] Unable to bind port %d - error %d\n", server->port, errno);
		return NULL;
	}

	// listen on socket/port
	// maximum of 1 pending connection
	if (listen(server->socket, 1) < 0) {
		fprintf(stderr, "[!] Unable to listen on port %d - error %d\n",server->port, errno);
		return NULL;
	}

	fprintf(stderr, "Listening on port %d\n", server->port);

	int addrlen = sizeof(server->addr);
	if((server->connection = accept(server->socket, (struct sockaddr*)&(server->addr), (socklen_t*)&addrlen)) < 0) {
		fprintf(stderr, "[!] Connection failure - error %d\n", errno);
		return NULL;
	}

	//read(server->connection, server->ip, sizeof(server->ip));
	fprintf(stdout, "port %d connected\n", server->port);
	server->connected = 1;
	return NULL;
}


