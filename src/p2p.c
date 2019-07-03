#include "p2p.h"

/* function initiates network by listening for incoming connections first (server side) */
int accept_p2p(p2p_struct *session, short int port) {
	int response;

	// creates server side socket
	if ((session->server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "Unable to create server side socket - error %d\n", errno);
		return session->server_socket;
	}

	// connection settings
	session->client_addr.sin_family = AF_INET;
	session->client_addr.sin_port = htons(port);
	session->client_addr.sin_addr.s_addr = INADDR_ANY;

	// binds socket to port
	if ((response = bind(session->server_socket, (struct sockaddr*)&(session->client_addr), sizeof(session->client_addr))) < 0) {
		fprintf(stderr, "Unable to bind port %d - error %d\n", port, errno);
		return response;
	}

	// listen on socket/port
	// maximum of 1 pending connection
	if ((response = listen(session->server_socket, 1)) < 0) {
		fprintf(stderr, "Unable to listen on port %d - error %d\n", port, errno);
		return response;
	}

	fprintf(stderr, "Establishing server side connection...");
	int addrlen = sizeof(session->client_addr);
	if((session->connection = accept(session->server_socket, (struct sockaddr*)&(session->client_addr), (socklen_t*)&addrlen)) < 0) {
		fprintf(stderr, "\nConnection failure - error %d\n", errno);
		return session->connection;
	}
	fprintf(stderr, "connected\n");

	return 1;
}

/* function connects to existing p2p network (client side) */
int connect_p2p(p2p_struct *session, short int port) {

	// creates client side socket
	if ((session->client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "Unable to create client side socket - error %d\n", errno);
		return session->client_socket;
	}

	// connection settings
	session->server_addr.sin_family = AF_INET;
	session->server_addr.sin_port = htons(port);

	// client side connection
	// continuous connection attempts after 2s delay
	fprintf(stderr, "Establishing client side connection...");
	int response = -1;
	for(int i = 0; i < 5 && response == -1; ++i) {
		response = connect(session->client_socket, (struct sockaddr*)&(session->server_addr), sizeof(session->server_addr));
		sleep(3);
	}
	if (response == -1) {
		fprintf(stderr, "\nUnable to connect - error %d\n", errno);
		return -1;
	}
	fprintf(stderr, "connected\n");

	return 1;
}

/* deallocates memory and closes sockets*/
void close_p2p(p2p_struct *session) {
	close(session->client_socket);
	close(session->server_socket);
	close(session->connection);
	free(session);
	return;
}

void *recieve_data(void *arg) {
	p2p_struct *session = (p2p_struct*)arg;
	char buffer[1024];
	int nbytes;
	do {
		nbytes = read(session->connection, buffer, sizeof(buffer));
		fprintf(stdout, "[*] %s\n", buffer);
	} while (nbytes > 0 && strcmp("X", buffer) != 0);
	return NULL;
}

/* recieves/prints incoming data from established connection(s) */
void *send_data(void *arg) {
	p2p_struct *session = (p2p_struct*)arg;
	char buffer[1024];
	int nbytes;
	do {
		fgets(buffer, 1024, stdin);
		buffer[strlen(buffer)-1] = '\0';
		send(session->client_socket, buffer, sizeof(buffer), 0);
	} while (nbytes > 0 && strcmp("X", buffer) != 0);
	return NULL;
}
