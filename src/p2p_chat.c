#include "p2p_chat.h"

/* returns 1 if all message details are the same, 0 otherwise */
int compare(message msg1, message msg2) {
	if (strcmp(msg1.sender, msg2.sender) == 0 && strcmp(msg1.origin, msg2.origin) == 0 && strcmp(msg1.msg, msg2.msg) == 0)
		return 1;
	return 0;
}

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
	message temp;
	while (1) {
		// if a new message is recieved
		// broadcast it to all connections
		if (compare(temp, recieved_message)) {
			// copies new message
			strcpy(temp.sender, recieved_message.sender);
			strcpy(temp.origin, recieved_message.origin);
			strcpy(temp.msg, recieved_message.msg);
			// send to server connection(s)
			for (int i = 0; i < nconn; ++i)
				if (session[i+1]->connected == 1)
					send(session[i+1]->connection, &temp, sizeof(temp), 0);
			// send to client conection
			if (session[0]->connected == 1)
				send(session[0]->socket, &temp, sizeof(temp), 0);
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
		strcpy(recieved_message.msg, buffer);
		fprintf(stdout, "[%s] %s\n", conn->ip, buffer);
	} while (nbytes > 0);

	fprintf(stdout, "[!] Unable to read data from %s - disconnected\n", conn->ip);
	conn->connected = 0;
	return NULL;
}