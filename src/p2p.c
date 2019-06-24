#include "p2p.h"

/* function initiates network by listening for incoming connections first (server side) */
int accept_p2p(p2p_struct *session, int port) {
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
int connect_p2p(p2p_struct *session, int port, char *addr) {
	
	// creates client side socket
	if ((session->client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "Unable to create client side socket - error %d\n", errno);
		return session->client_socket;
	}

	// sets connection settings
	session->server_addr.sin_family = AF_INET;
	session->server_addr.sin_port = htons(port);
	if ((inet_pton(AF_INET, addr, &session->server_addr.sin_addr)) <= 0) {
		fprintf(stderr, "Invalid IP address - error %d\n", errno);
		return -1;
	}

	// creates connection
	fprintf(stderr, "Establishing client side connection...");
	if (connect(session->client_socket, (struct sockaddr*)&(session->server_addr), sizeof(session->server_addr)) < 0) {
		fprintf(stderr, "\nUnable to connect - error %d\n", errno);
		return -1;
	}
	fprintf(stderr, "connected\n");

	return 1;
}

/* sends user data to connection */
int transfer_data(p2p_struct *session) {

	char buffer[1024];
	int nbytes;
	do {
	 	fprintf(stdout, ">");
		fgets(buffer, 1024, stdin);
		buffer[strlen(buffer)-1] = '\0';
		nbytes = send(session->client_socket, buffer, sizeof(buffer), 0);
	} while (nbytes > 0);

	return 1;
}

/* recieves data from connection */
int recieve_data(p2p_struct *session) {
	
	char buffer[1024];
	int nbytes;
	do {
		nbytes = read(session->connection, buffer, sizeof(buffer));
		fprintf(stdout, "[*] %s\n", buffer);
	} while (nbytes > 0);
	return 1;
}

/* deallocates memory and closes sockets*/
void close_p2p(p2p_struct *session) {
	close(session->client_socket);
	close(session->server_socket);
	free(session);
	return;
}
