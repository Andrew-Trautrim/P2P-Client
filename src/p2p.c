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
		return response;
	}

	// local info
	char host_buffer[256];
	char *ip;
	struct hostent *host_entry;
	int host_name;
	host_name = gethostname(host_buffer, sizeof(host_buffer));
	host_entry = gethostbyname(host_buffer);
	ip = inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[0]));
	send(client->socket, ip, sizeof(ip)+1, 0);

	return 1;
}

/* sends user input to all established connections */
int send_data(p2p_struct **server, p2p_struct *client) {

	char buffer[1024];
	int nbytes;

	do {
		// input
		fgets(buffer, 1024, stdin);
		buffer[strlen(buffer)-1] = '\0';
		
		// send to server connection(s)
		for (int i = 0; i < nconn; ++i) {
			if (server[i]->connection != 0) {
				nbytes = send(server[i]->connection, buffer, sizeof(buffer), 0);
				if (nbytes < 0) {
					fprintf(stderr, "[!] Unable to send data to %s\n", server[i]->ip);
				}
			}
		}

		// send to client connection(s)
		if (client->socket != 0) {
			nbytes = send(client->socket, buffer, sizeof(buffer), 0);
			if (nbytes < 0) {
				fprintf(stderr, "[!] Unable to send data to %s\n", client->ip);
			}
		}
	} while (strcmp("X", buffer) != 0);

	if (nbytes < 0)
		fprintf(stderr, "[!] Unable to send data\n");
	return -1;
}

/* allocates memory for the session and sets the port */
p2p_struct *init_p2p(unsigned short port) {
	p2p_struct *session = (p2p_struct*)calloc(1, sizeof(p2p_struct));
	session->socket = 0;
	session->connection = 0;
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

	read(server->connection, server->ip, sizeof(server->ip));
	fprintf(stderr, "%s connected on port %d\n", server->ip, server->port);
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
		if (strcmp("X", buffer) == 0) {
			fprintf(stdout, "[!] %s disconnected\n", server->ip);
			return NULL;
		}
		fprintf(stdout, "[%s] %s\n", server->ip, buffer);
	} while (nbytes > 0);

	fprintf(stdout, "[!] %s disconnected\n", server->ip);
	return NULL;
}

/* reads data from server side connections */
void *read_server(void *arg) {
	p2p_struct *client = (p2p_struct*)arg;
	char buffer[1024];
	int nbytes;
	do {
		nbytes = read(client->socket, buffer, sizeof(buffer));
		if (strcmp("X", buffer) == 0) {
			fprintf(stdout, "[!] %s diconnected\n", client->ip);
			return NULL;
		}
		fprintf(stdout, "[%s] %s\n", client->ip, buffer);
	} while (nbytes > 0);

	fprintf(stdout, "[!] %s disconnected\n", client->ip);
	return NULL;
}
