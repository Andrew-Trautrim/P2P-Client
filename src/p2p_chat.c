#include "p2p_chat.h"

/* sends user input to all established connections */
int send_data() {
	
	message msg;
	int nbytes;
	int flag = 1;

	do {

		flag = 0;

		// input
		fgets(msg.msg, 1024, stdin);
		msg.msg[strlen(msg.msg)-1] = '\0';
	
		// send to all active connection(s)
		for (int i = 0, n = cconn+sconn; i < n; ++i) {
			if (session[i]->connected == 1 && session[i]->active == 1) {
				strcpy(msg.origin, inet_ntoa(session[i]->addr.sin_addr));
				if (i < cconn) { // client(s)
					nbytes = send(session[i]->socket, &msg, sizeof(msg), 0);
				} else { // server(s)
					nbytes = send(session[i]->connection, &msg, sizeof(msg), 0);
				}
			}

			// if atleast one connection is still maintained, continue
			// exit otherwise
			if (!(session[i]->connected == 0 && session[i]->active == 1))
				flag = 1;
		}
	} while (nbytes >= 0 && strcmp("X", msg.msg) != 0 && flag == 1);

	if (nbytes < 0) {
		fprintf(stderr, "[!] Unable to send data\n");
	}
	return 1;
}

/* reads data from client/server side connections */
void *read_data(void *arg) {

	p2p_struct *conn = (p2p_struct*)arg;
	int nbytes;
	int index = 0;

	message msg;

	for (int i = 0, n = cconn+sconn; i < n; ++i) {
		if (session[i] == conn) {
			index = i;
			break;
		}
	}

	do {

		// reads data as either server or client
		if(index < cconn)
			nbytes = read(conn->socket, &msg, sizeof(msg));
		else
			nbytes = read(conn->connection, &msg, sizeof(msg));

		// 'X' terminates connection
		if (strcmp("X", msg.msg) == 0) {
			fprintf(stdout, "[!] %s disconnected\n", msg.origin);
			close(session[index]->socket);
			close(session[index]->connection);
			conn->connected = 0;
			return NULL;
		}

		// broadcast data to other connections
		for (int i = 0, n = cconn+sconn; i < n; ++i) {
			if (i != index && session[i]->active == 1 && session[i]->connected == 1) {
				if (i < cconn) { // client(s)
					nbytes = send(session[i]->socket, &msg, sizeof(msg), 0);
				} else if (session) { // server(s)
					nbytes = send(session[i]->connection, &msg, sizeof(msg), 0);
				}
			}
		}

		fprintf(stdout, "[%s:%d] %s\n", msg.origin, conn->port, msg.msg); // display message output
	} while (nbytes > 0);

	fprintf(stdout, "[!] Unable to read data from %s on port %d - disconnected\n", msg.origin, conn->port);
	conn->connected = 0;
	return NULL;
}