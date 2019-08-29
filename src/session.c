#include "p2p.h"
#include "p2p_chat.h"

int get_usage(); // gets program usage from user
void print_usage(); // displays usage options
void *manage_server(void *arg); // manages connections in the background

int main(int argc, char **argv) {

	static char *options = "a:hln:p:t:cfr";

	nconn = 2; // default number of connections is 2
	int connect = 0;
	int listen = 0;
	int use = 0;
	int opt, nbytes;
	int local_port = 18, target_port = 18; // default port is 18
	char *addr = NULL;

	// command line argument(s)
	while((opt = getopt(argc, argv, options)) != -1) {
		switch (opt) {
			case 'a':
				connect = 1;
				addr = optarg;
				break;
			case 'h':
				print_usage();
				return 1;
			case 'l':
				listen = 1;
				break;
			case 'n':
				nconn = atoi(optarg);
				break;
			case 'p':
				local_port = atoi(optarg);
				break;
			case 't':
				target_port = atoi(optarg);
				break;
			case 'c':
				use = 1;
				break;
			case 'f':
				use = 2;
				break;
			case 'r':
				use = 3;
				break;
			case '?':
				fprintf(stderr, "Improper usage, use \'-h\' for help\n");
				return -1;
			default:
				// getopt error, displays own error message
				return -1;
		}
	}

	// network options not specified, program must listen and/or connect
	if (!listen && !connect) {
		fprintf(stderr, "Improper usage, use \'-l\' and/or \'-a\' to connect\ntype \'-h\' for help\n");
		return -1;
	}
	// usage not specified, program must either deploy chatroom, file transfer, or remote command line
	if (use == 0) {
		fprintf(stderr, "Usage not specified, use \'-c\', \'-f\', or \'r\'\ntype \'h\' for help\n");
		return -1;
	}

	// set local information
	char host_buffer[256];
	struct hostent *host_entry;
	int host_name;
	host_name = gethostname(host_buffer, sizeof(host_buffer));
	host_entry = gethostbyname(host_buffer);
	local_ip = inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[0]));

	// initiate connections
	p2p_struct *session[nconn+1];
	session[0] = init_p2p(target_port);
	for (int i = 0; i < nconn; ++i)
		session[i+1] = init_p2p(local_port + i);

	// multithreading, listening on multiple ports simultaneously
	pthread_t slisten[nconn];
	if (listen) 
		for (int i = 0; i < nconn; ++i)
			pthread_create(&slisten[i], NULL, accept_p2p, session[i+1]);
	
	// connect to specified address
	if (connect) {
		if (inet_pton(AF_INET, addr, &session[0]->addr.sin_addr) <= 0) {
			fprintf(stderr, "[!] Invalid address - error %d\n", errno);
			close_p2p(session);
			return -1;
		}

		if (connect_p2p(session[0]) < 0) {
			close_p2p(session);
			return -1;
		}
		strcpy(session[0]->ip, addr);
		printf("TEST: %s - %s\n", addr, session[0]->ip);
		fprintf(stderr, "Connected to %s\n", addr);
	}

	// Chat Room
	if (use == 1) {
		fprintf(stdout, "Chat Room, type 'X' to exit:\n");

		pthread_t broadcast, manage, read_client;
		if (listen)
			pthread_create(&manage, NULL, manage_server, session);

		if (connect)
			pthread_create(&read_client, NULL, read_data, session[0]);

		// broadcasts recieved data to all connections
		pthread_create(&broadcast, NULL, broadcast_data, session);
		// sends data to all connections
		send_data(session);
	}
	// TODO: File Transfer
	else if (use == 2) {
		fprintf(stdout, "Usage not available: file transfer\n");
	}
	// TODO: Remote CLI
	else {
		fprintf(stdout, "Usage not available: remote command line\n");
	}

	close_p2p(session);
	return 0;
}

/* prints help message for proper usage */
void print_usage() {
	fprintf(stdout, "usage: ./session <options>\n");
	fprintf(stdout, "network options:\n");
	fprintf(stdout, "	-a address : connect to target address\n");
	fprintf(stdout, "	-h	   : display help options\n");
	fprintf(stdout, "	-l	   : listen for incoming connections\n");
	fprintf(stdout, "	-n max #   : maximum number of incoming connections\n");
	fprintf(stdout, "	-p port	   : local port for accepting connections\n");
	fprintf(stdout, "	-t port    : target port for connecting\n");
	fprintf(stdout, "\ninterface options:\n");
	fprintf(stdout, "	-c	   : chat room\n");
	fprintf(stdout, "	-f	   : file transfer\n");
	fprintf(stdout, "	-r	   : remote command line interface\n");
	return;
}

/*
 * manages all server side connections
 * creates a thread to read incoming data if a connections is made
 */
void *manage_server(void *arg) {

	p2p_struct **session = (p2p_struct**)arg;
	pthread_t read[nconn];
	int flag = 1;

	while (flag) {
		for (int i = 0; i < nconn; ++i) {
			flag = 0;
			// if connection is made, read incoming data
			if (session[i+1]->connected == 1 && session[i+1]->active == 0) {
				session[i+1]->active = 1;
				pthread_create(&read[i], NULL, read_data, session[i+1]);
			}

			// if atleast one connection is still maintained, continue
			// exit otherwise
			if (!(session[i+1]->connected == 0 && session[i+1]->active == 1))
				flag = 1;
		}
	}
	return NULL;
}