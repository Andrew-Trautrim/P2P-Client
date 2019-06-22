#include "p2p.h"

int main(int argc, char **argv) {

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <address> <port>\n", argv[0]);
		return -1;
	}
	char *addr = argv[1];
	int port = (argv[2] != NULL) ? atoi(argv[2]) : PORT; // default port is 8080
	
	p2p_struct *session;
	init_p2p(session);

	int response;
	if (LISTEN) {
		if ((response = accept_p2p(session, port)) < 0)
			return response;
	} else {
		if ((response = connect_p2p(session, port, addr)) < 0)
			return response;
	}

	close_p2p(session);
	return 0;
}
