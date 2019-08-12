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
		for (int i = 0; i < nconn; ++i) {
			if (server[i]->active == 1) {
				nbytes = send(server[i]->connection, buffer, sizeof(buffer), 0);
			}
		}
		if (client->active == 1) {
			nbytes = send(client->socket, buffer, sizeof(buffer), 0);
		}
	} while (nbytes >= 0 && strcmp("X", buffer) != 0);

	if (nbytes < 0) {
		fprintf(stderr, "[!] Unable to send data\n");
	}
	return 1;
}

/* reads data from client side connections */
void *read_server(void *arg) {
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
void *read_client(void *arg) {
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