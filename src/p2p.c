#include "p2p.h"

/* function connects to existing p2p network (client side) */
int connect_p2p(p2p_struct *client) {

	// creates client side socket
	if ((client->socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "[!] Unable to create client side socket - error %d\n", errno);
		return client->socket;
	}

	// connection settings
	client->addr.sin_family = AF_INET;
	client->addr.sin_port = htons(client->port);

	// client side connection
	int response = -1;
	for(int i = 0; i < 5 && response == -1; ++i) {
		response = connect(client->socket, (struct sockaddr*)&(client->addr), sizeof(client->addr));
		sleep(2);
	}
	if (response == -1) {
		fprintf(stderr, "[!] Unable to connect - error %d\n", errno);
		return -1;
	}

	fprintf(stderr, "Connection established\n");
	return 1;
}


/*  sends user input to all established connections */
int send_data(p2p_struct *session) {
	char buffer[1024];
	int nbytes;
	do {
		fgets(buffer, 1024, stdin);
		buffer[strlen(buffer)-1] = '\0';
		if (session->socket != 0)
			nbytes = send(session->socket, buffer, sizeof(buffer), 0);
		else if (session->connection != 0)
			nbytes = send(session->connection, buffer, sizeof(buffer), 0);
	} while (nbytes >= 0 && strcmp("X", buffer) != 0);
	if (nbytes < 0)
		fprintf(stderr, "[!] Unable to send data\n");
	return -1;
}

/* allocates memory for the session and sets the connection count */
p2p_struct *init_p2p(unsigned short port) {
	p2p_struct *session = (p2p_struct*)calloc(1, sizeof(p2p_struct));
	session->socket = 0, session->connection = 0;
	session->port = port;
	return session;
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

	int addrlen = sizeof(server->addr);
	if((server->connection = accept(server->socket, (struct sockaddr*)&(server->addr), (socklen_t*)&addrlen)) < 0) {
		fprintf(stderr, "[!] Connection failure - error %d\n", errno);
		return NULL;
	}
	
	fprintf(stderr, "Connection established\n");
	return NULL;
}

/* deallocates memory and closes sockets*/
void close_p2p(p2p_struct *session) {
	close(session->socket);
	close(session->connection);
	free(session);
	return;
}

/* reads data from client side connections */
void *read_client(void *arg) {
	p2p_struct *server = (p2p_struct*)arg;
	char buffer[1024];
	int nbytes;
	do {
		nbytes = read(server->connection, buffer, sizeof(buffer));
		if (strcmp("X", buffer) == 0)
			return NULL;
		fprintf(stdout, "[*] %s\n", buffer);
	} while (nbytes > 0);
	return NULL;
}

/* reads data from server side connections */
void *read_server(void *arg) {
	p2p_struct *client = (p2p_struct*)arg;
	char buffer[1024];
	int nbytes;
	do {
		nbytes = read(client->socket, buffer, sizeof(buffer));
		if (strcmp("X", buffer) == 0)
			return NULL;
		fprintf(stdout, "[*] %s\n", buffer);
	} while (nbytes > 0);
	return NULL;
}
