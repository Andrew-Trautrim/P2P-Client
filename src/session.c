#include "p2p.h"

void print_usage();

int main(int argc, char **argv) {

	static char *options = "a:hlp:";

	int opt;
	int listen = 0;
	int port = 8080;
	char *addr = NULL;
	p2p_struct *session;

	// proper usage
	if (argc < 2) {
		fprintf(stderr, "usage: %s <options>\ntype \'-h\' for help\n", argv[0]);
		return -1;
	}

	// command line argument(s)
	while((opt = getopt(argc, argv, options)) != -1) {
		switch (opt) {
			case 'a':
				addr = optarg;
				break;
			case 'h':
				print_usage();
				return -1;
			case 'l':
				listen = 1;
				break;
			case 'p':
				port = atoi(optarg);
				break;
			case '?':
				fprintf(stderr, "Improper usage, use \'-h\' for help\n");
				return -1;
			default:
				// getopt error, displays own error message
				return -1;
		}
	}

	session = (p2p_struct *)calloc(1, sizeof(p2p_struct));

	// creating connection
	if (listen) {
		if (accept_p2p(session, port) < 0) {
			close_p2p(session);
			return -1;
		}
		if (connect_p2p(session, port, addr) < 0) {
			close_p2p(session);
			return -1;
		}
	} else {
		if (connect_p2p(session, port, addr) < 0) {
			close_p2p(session);
			return -1;
		}
		if (accept_p2p(session, port) < 0) {
			close_p2p(session);
			return -1;
		}
	}
	// send/recieve data from connetion
	transfer_data(session);

	close_p2p(session);
	return 0;
}

/* prints help message for proper usage */
void print_usage() {
	fprintf(stdout, "usage: ./session <options>\n\n");
	fprintf(stdout, "Options:\n");
	fprintf(stdout, "	-a address : target address\n");
	fprintf(stdout, "	-h	   : display help options\n");
	fprintf(stdout, "	-l	   : listen for incoming connections\n");
	fprintf(stdout, "	-p port	   : port\n");
	return;
}
