#include "p2p_chat.h"

/* sends user input to all established connections */
int send_data(p2p_struct **session) {

	char buffer[1024];
	int nbytes;

	do {
		// input
		fgets(buffer, 1024, stdin);
		buffer[strlen(buffer)-1] = '\0';
	
		// send to server connection(s)
		for (int i = 0; i < nconn; ++i)
			if (session[i+1]->connected == 1)
				nbytes = send(session[i+1]->connection, buffer, sizeof(buffer), 0);
		// send to client conection
		if (session[0]->connected == 1) 
			nbytes = send(session[0]->socket, buffer, sizeof(buffer), 0);

	} while (nbytes >= 0 && strcmp("X", buffer) != 0);

	if (nbytes < 0) {
		fprintf(stderr, "[!] Unable to send data\n");
	}
	return 1;
}

/* broadcasts recieved messages to other connections */
void *broadcast_data(void *arg) {
	p2p_struct **session = (p2p_struct**)arg;
	char msg[MSG_LEN];
	while (1) {
		// if a new message is recieved
		// broadcast it to all connections
		if (strcmp(msg, recieved_message) != 0) {
			// copies new message
			strcpy(msg, recieved_message);
			// send to server connection(s)
			for (int i = 0; i < nconn; ++i)
				if (session[i+1]->connected == 1)
					send(session[i+1]->connection, msg, sizeof(msg), 0);
			// send to client conection
			if (session[0]->connected == 1)
				send(session[0]->socket, msg, sizeof(msg), 0);
		}
	}
	return NULL;
}

/* reads data from client/server side connections */
void *read_data(void *arg) {
	p2p_struct *conn = (p2p_struct*)arg;
	char buffer[1024];
	int nbytes;
	do {
		// reads data as either server or client
		if(conn->connection == 0) 
			nbytes = read(conn->socket, buffer, sizeof(buffer));
		else
			nbytes = read(conn->connection, buffer, sizeof(buffer));

		if (strcmp("X", buffer) == 0) {
			fprintf(stdout, "[!] %s disconnected\n", conn->ip);
			conn->connected = 0;
			return NULL;
		}
		strcpy(recieved_message, buffer);
		fprintf(stdout, "[%s] %s\n", conn->ip, buffer);
	} while (nbytes > 0);

	fprintf(stdout, "[!] Unable to read data from %s - disconnected\n", conn->ip);
	conn->connected = 0;
	return NULL;
}