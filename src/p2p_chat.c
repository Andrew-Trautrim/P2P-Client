#include "p2p_chat.h"

/* sends user input to all established connections */
int send_data(p2p_struct **session) {
	
	char buffer[1024];
	int nbytes;

	do {

		// input
		fgets(buffer, 1024, stdin);
		buffer[strlen(buffer)-1] = '\0';
	
		// send to all active connection(s)
		for (int i = 0, n = cconn+sconn; i < n; ++i) {
			if (session[i]->connected == 1 && session[i]->active == 1) {
				if (i < cconn) { // client(s)
					nbytes = send(session[i]->socket, &buffer, sizeof(buffer), 0);
				} else { // server(s)
					nbytes = send(session[i]->connection, &buffer, sizeof(buffer), 0);
				}
			}
		}

	} while (nbytes >= 0 && strcmp("X", buffer) != 0);

	if (nbytes < 0) {
		fprintf(stderr, "[!] Unable to send data\n");
	}
	return 1;
}

/* reads data from client/server side connections */
void *read_data(void *arg) {

	p2p_struct *conn = (p2p_struct*)arg;
	char buffer[1024];
	int nbytes;
	int index = 0;
	for (int i = 0, n = cconn+sconn; i < n; ++i) {
		if (strcmp(session[i]->ip, conn->ip) == 0 && session[i]->ip == conn->ip) {
			index = i;
			break;
		}
	}

	do {

		// reads data as either server or client
		if(index < cconn)
			nbytes = read(conn->socket, &buffer, sizeof(buffer));
		else
			nbytes = read(conn->connection, &buffer, sizeof(buffer));

		// broadcast data to other connections
		/*
		for (int i = 0, n = cconn+sconn; i < n; ++i) {
			if (i != index && session[i]->active == 1 && session[i]->connected == 1) {
				if (i < cconn) { // client(s)
					nbytes = send(session[i]->socket, &buffer, sizeof(buffer), 0);
				} else if (session){ // server(s)
					nbytes = send(session[i]->connection, &buffer, sizeof(buffer), 0);
				}
			}
		}
		*/

		if (strcmp("X", buffer) == 0) {
			fprintf(stdout, "[!] %s disconnected\n", conn->ip);
			conn->connected = 0;
			return NULL;
		}

		fprintf(stdout, "[%s:%d] %s\n", conn->ip, conn->port, buffer);
	} while (nbytes > 0);

	fprintf(stdout, "[!] Unable to read data from %s on port %d - disconnected\n", conn->ip, conn->port);
	conn->connected = 0;
	return NULL;
}