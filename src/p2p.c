#include "p2p.h"

/* function initiates network by listening for incoming connections (server side) */
int accept_p2p(p2p_struct *session, short int port, unsigned short n) {

	// creates server side socket
	if ((session->socket[n] = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "Unable to create server side socket - error %d\n", errno);
		return session->socket[n];
	}

	// connection settings
	session->addr[n].sin_family = AF_INET;
	session->addr[n].sin_port = htons(port);
	session->addr[n].sin_addr.s_addr = INADDR_ANY;

	// binds socket to port
	if (bind(session->socket[n], (struct sockaddr*)&(session->addr[n]), sizeof(session->addr[n])) < 0) {
		fprintf(stderr, "Unable to bind port %d - error %d\n", port, errno);
		return -1;
	}

	// listen on socket/port
	// maximum of 1 pending connection
	if (listen(session->socket[n], 1) < 0) {
		fprintf(stderr, "Unable to listen on port %d - error %d\n", port, errno);
		return -1;
	}

	fprintf(stderr, "Establishing server side connection...");
	int addrlen = sizeof(session->addr[n]);
	if((session->connection[n] = accept(session->socket[n], (struct sockaddr*)&(session->addr[n]), (socklen_t*)&addrlen)) < 0) {
		fprintf(stderr, "\nConnection failure - error %d\n", errno);
		return session->connection[n];
	}
	fprintf(stderr, "connected\n");

	return 1;
}

/* function connects to existing p2p network (client side) */
int connect_p2p(p2p_struct *session, short int port, unsigned short n) {

	// creates client side socket
	if ((session->socket[n] = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "Unable to create client side socket - error %d\n", errno);
		return session->socket[n];
	}

	// connection settings
	session->addr[n].sin_family = AF_INET;
	session->addr[n].sin_port = htons(port);

	// client side connection
	// continuous connection attempts after 2s delay
	fprintf(stderr, "Establishing client side connection...");
	int response = -1;
	for(int i = 0; i < 5 && response == -1; ++i) {
		response = connect(session->socket[n], (struct sockaddr*)&(session->addr[n]), sizeof(session->addr[n]));
		sleep(3);
	}
	if (response == -1) {
		fprintf(stderr, "\nUnable to connect - error %d\n", errno);
		return -1;
	}
	fprintf(stderr, "connected\n");

	return 1;
}

/* allocates memory for the session and sets the connection count */
p2p_struct *init_p2p() {
	p2p_struct *session = (p2p_struct*)calloc(1, sizeof(p2p_struct));
	session->nconn = 0;
	return session;
}

/* deallocates memory and closes sockets*/
void close_p2p(p2p_struct *session) {
	for (int i = 0; i <= session->nconn; ++i) {
		close(session->socket[i]);
		close(session->connection[i]);
	}
	free(session);
	return;
}

/* listens for incoming connections */
void *listen_p2p(void *arg) {
	return NULL;	
}

/* reads incoming data from connections */
void *recieve_data(void *arg) {
	p2p_struct *session = (p2p_struct*)arg;
	char buffer[1024];
	int nbytes;
	do {
		for (int i = 0; i < session->nconn; ++i)
			nbytes = read(session->socket[i], buffer, sizeof(buffer));
		if (strcmp("X", buffer) == 0)
			return NULL;
		fprintf(stdout, "[*] %s\n", buffer);
	} while (nbytes > 0);
	return NULL;
}

/*  sends user input to all established connections */
void *send_data(void *arg) {
	p2p_struct *session = (p2p_struct*)arg;
	char buffer[1024];
	int nbytes;
	do {
		fgets(buffer, 1024, stdin);
		buffer[strlen(buffer)-1] = '\0';
		for (int i = 0; i < session->nconn; ++i)
			nbytes = send(session->socket[i], buffer, sizeof(buffer), 0);
	} while (nbytes > 0 && strcmp("X", buffer) != 0);
	return NULL;
}
