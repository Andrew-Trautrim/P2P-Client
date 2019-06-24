#include "p2p.h"

int main(int argc, char **argv) {

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <address> <port>\n", argv[0]);
		return -1;
	}
	char *addr = argv[1];
	int port = (argv[2] != NULL) ? atoi(argv[2]) : PORT; // default port is 8080
	p2p_struct *session = (p2p_struct *)calloc(1, sizeof(p2p_struct));

	// creating connection
	if (LISTEN) {
		if (accept_p2p(session, port) < 0) {
			close_p2p(session);
			return -1;
		}
	} else {
		if (connect_p2p(session, port, addr) < 0) {
			close_p2p(session)
			return -1;
		}
	}

	close_p2p(session);
	return 0;
}
