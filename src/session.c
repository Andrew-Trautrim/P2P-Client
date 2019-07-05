#include "p2p.h"

void print_usage(); // displays usage options

int main(int argc, char **argv) {

	static char *options = "a:hln:p:t:";

	int opt, nbytes;
	int nconn = 2; // default number of connections is 2
	int local_port = 18, target_port = 18; // default port is 18
	int connect = 0, listen = 0;
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
				return -1;
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
			case '?':
				fprintf(stderr, "Improper usage, use \'-h\' for help\n");
				return -1;
			default:
				// getopt error, displays own error message
				return -1;
		}
	}

	// program must either listen or connect or both
	if (!listen && !connect) {
		fprintf(stderr, "Improper usage, use \'-l\' and/or \'-a\' to connect\ntype \'-h\' for help\n");
		return -1;
	}

	p2p_struct *client;
	p2p_struct *server[nconn];

	// multithreading
	// listening on multiple ports simultaneously
	// max of N connections
	for (int i = 0; i < nconn; ++i)
		server[i] = init_p2p(local_port + i);
	pthread_t listening[nconn];
	if (listen) {
		for (int i = 0; i < nconn; ++i) {
			fprintf(stderr, "listening on port %d\n", local_port + i);
			pthread_create(&listening[i], NULL, accept_p2p, server[i]);
		}
	}
	
	// connect to specified address
	// only 1 connection allowed (so far)
	client = init_p2p(target_port);
	if (connect) {

		if (inet_pton(AF_INET, addr, &client->addr.sin_addr) <= 0) {
			fprintf(stderr, "[!] Invalid address - error %d\n", errno);
			close_p2p(client);
			for (int i = 0; i < nconn; ++i)
				close_p2p(server[i]);
			return -1;
		}

		if (connect_p2p(client) < 0) {
			close_p2p(client);
			for (int i = 0; i < nconn; ++i)
				close_p2p(server[i]);
			return -1;
		}
	}
	
	fprintf(stdout, "Session (type \'X\' to exit): \n");

	close_p2p(client);
	for (int i = 0; i < nconn; ++i) {
		close_p2p(server[i]);
	}
	return 0;
}

/* prints help message for proper usage */
void print_usage() {
	fprintf(stdout, "usage: ./session <options>\n\n");
	fprintf(stdout, "options:\n");
	fprintf(stdout, "	-a address : connect to target address\n");
	fprintf(stdout, "	-h	   : display help options\n");
	fprintf(stdout, "	-l	   : listen for incoming connections\n");
	fprintf(stdout, "	-n max #   : maximum number of incoming connections\n");
	fprintf(stdout, "	-p port	   : local port for accepting connections\n");
	fprintf(stdout, "	-t port    : target port for connecting\n");
	return;
}
