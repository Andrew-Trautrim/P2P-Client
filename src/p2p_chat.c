#include "p2p_chat.h"

/* returns 1 if all message details are the same, 0 otherwise */
int compare(message msg1, message msg2) {

	if (msg1.relay == 1 && msg2.relay == 1 && strcmp(msg1.sender, msg2.sender) == 0 && strcmp(msg1.origin, msg2.origin) == 0 && strcmp(msg1.msg, msg2.msg) == 0)
		return 1;

	return 0;
}

/* sends user input to all established connections */
int send_data(p2p_struct **session) {

	message msg;
	int nbytes;

	do {

		// input
		fgets(msg.msg, 1024, stdin);
		msg.msg[strlen(msg.msg)-1] = '\0';
		strcpy(msg.sender, local_ip);
		strcpy(msg.origin, local_ip);
	
		// send to all active connection(s)
		for (int i = 0, n = cconn+sconn; i < n; ++i) {
			if (session[i]->connected == 1) {
				if (i < cconn) // client(s)
					nbytes = send(session[i]->socket, &msg, sizeof(msg), 0);
				else // server(s)
					nbytes = send(session[i]->connection, &msg, sizeof(msg), 0);
			}
		}

	} while (nbytes >= 0 && strcmp("X", msg.msg) != 0);

	if (nbytes < 0) {
		fprintf(stderr, "[!] Unable to send data\n");
	}
	return 1;
}

/* broadcasts recieved messages to other connections */
void *broadcast_data(void *arg) {

	p2p_struct **session = (p2p_struct**)arg;
	message msg;

	// temporary values to initiate the message broadcast
	strcmp(recieved_message.sender, "0");
	strcmp(recieved_message.origin, "0");
	strcmp(recieved_message.msg, "0");
	strcmp(msg.sender, "1");
	strcmp(msg.origin, "1");
	strcmp(msg.msg, "1");

	while (1) {

		// if a new message is recieved
		// broadcast it to all connections
		if (compare(msg, recieved_message)) {

			// copies new message
			strcpy(msg.sender, recieved_message.sender);
			strcpy(msg.origin, recieved_message.origin);
			strcpy(msg.msg, recieved_message.msg);

			for (int i = 0, n=sconn+cconn; i < n; ++i) {

				// send to server connection(s)
				if (session[i]->connected == 1 && strcmp(session[i]->ip, msg.sender) != 0 && strcmp(session[i]->ip, msg.origin) != 0) { // message isnt broadcasted to the sender or the original sender
					if (i < cconn) { // client
						send(session[i]->socket, &msg, sizeof(msg), 0);
					} else { // server
						send(session[i]->connection, &msg, sizeof(msg), 0);
					}
				}
			}
		}
		sleep(0.5);
	}
	return NULL;
}

/* reads data from client/server side connections */
void *read_data(void *arg) {

	p2p_struct *conn = (p2p_struct*)arg;
	message msg;
	int nbytes;

	do {

		// reads data as either server or client
		if(conn->connection == 0) 
			nbytes = read(conn->socket, &msg, sizeof(msg));
		else
			nbytes = read(conn->connection, &msg, sizeof(msg));

		if(strcmp("UNKNOWN", conn->ip))
			strcpy(conn->ip, msg.origin);

		if (strcmp("X", msg.msg) == 0) {
			fprintf(stdout, "[!] %s disconnected\n", conn->ip);
			conn->connected = 0;
			return NULL;
		}

		strcpy(recieved_message.sender, msg.sender);
		strcpy(recieved_message.origin, msg.origin);
		strcpy(recieved_message.msg, msg.msg);
		fprintf(stdout, "[%s:%d] %s\n", msg.origin, conn->port, msg.msg);
	} while (nbytes > 0);

	fprintf(stdout, "[!] Unable to read data from %s - disconnected\n", conn->ip);
	conn->connected = 0;
	return NULL;
}