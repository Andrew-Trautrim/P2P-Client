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
		fprintf(stderr, "Invalid IP address: %s - error %d\n", addr, errno);
		return -1;
	}

	// client side connection
	// continuous connection attempts after 2s delay
	fprintf(stderr, "Establishing client side connection...");
	int response = -1;
	for(int i = 0; i < 5 && response == -1; ++i) {
		response = connect(session->client_socket, (struct sockaddr*)&(session->server_addr), sizeof(session->server_addr));
		delay(2000000);
	}
	if (response == -1) {
		fprintf(stderr, "\nUnable to connect - error %d\n", errno);
		return -1;
	}
	fprintf(stderr, "connected\n");

	return 1;
}

/* transfers data back and forth from connections */
int transfer_data(p2p_struct *session) {
	char input[1024];
	char output[1024];
	int nbytes;
	do {
		// attempts to read incoming data first
		if ((nbytes = read(session->connection, input, sizeof(input))) > 0) {
			fprintf(stdout, "[*] %s\n", input);
		// sends data otherwise
		} else {
			fprintf(stdout, "> ");
			fgets(input, 1024, stdin);
			input[strlen(input)-1] = '\0';
			if (strlen(input) > 0)
				nbytes = send(session->client_socket, input, sizeof(input), 0);
		}
	} while (nbytes > 0);
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

/* function delays a specified interval */
void delay(int ticks) {
	clock_t start = clock();
	while(clock() < (start + ticks))
		;
}
