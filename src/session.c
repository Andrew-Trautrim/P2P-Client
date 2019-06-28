#include "p2p.h"

int main(int argc, char **argv) {

	static char *options = "a:lp:";

	int opt;
	int listen = 0;
	int port = 8080;
	char *addr;
	p2p_struct *session;

	// proper usage
	if (argc < 2) {
		fprintf(stderr, "Improper usage: %s [options]\n", argv[0]);
		return -1;
	}

	// command line argument(s)
	while((opt = getopt(argc, argv, options)) != -1) {
		switch (opt) {
			case 'a':
				addr = optarg;
				break;
			case 'l':
				listen = 1;
				break;
			case 'p':
				port = atoi(optarg);
				break;
			default:
				fprintf(stderr, "Invalid argument, \'-%c\'\n", opt);
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


