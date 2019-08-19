#include "p2p_chat.h"

/* sends user input to all established connections */
int send_data(p2p_struct **server, p2p_struct *client) {

	char buffer[1024];
	int nbytes;

	do {
		// input
		fgets(buffer, 1024, stdin);
		buffer[strlen(buffer)-1] = '\0';
	
		// send to server connection(s)
		for (int i = 0; i < nconn; ++i)
			if (server[i]->connected == 1)
				nbytes = send(server[i]->connection, buffer, sizeof(buffer), 0);
		if (client->connected == 1) 
			nbytes = send(client->socket, buffer, sizeof(buffer), 0);

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
		fprintf(stdout, "[%s] %s\n", conn->ip, buffer);
	} while (nbytes > 0);

	fprintf(stdout, "[!] Unable to read data from %s - disconnected\n", conn->ip);
	conn->connected = 0;
	return NULL;
}