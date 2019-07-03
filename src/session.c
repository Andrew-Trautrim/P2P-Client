#include "p2p.h"

void print_usage();

int main(int argc, char **argv) {

	static char *options = "a:hlp:";

	int opt, nbytes;
	int listen = 0;
	int port = 8080;
	char *addr = NULL;
	p2p_header *header;
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
	header = (p2p_header*)calloc(1, sizeof(p2p_header));

	// creating connection
	if (listen) {
		if (accept_p2p(session, port) < 0) {
			close_p2p(session);
			free(header);
			return -1;
		}
		
		if (nbytes = read(session->connection, header, sizeof(p2p_header)) < 0) {
			fprintf(stderr, "Unable to read incoming data - error %d\n", errno);
			close_p2p(session);
			free(header);
			return -1;
		}

		session->client_addr.sin_addr.s_addr = header->local_addr.s_addr;
		if (connect_p2p(session, port) < 0) {
			close_p2p(session);
			free(header);
			return -1;
		}
	} else {
		if (inet_pton(AF_INET, addr, &session->server_addr.sin_addr) <= 0) {
			fprintf(stderr, "Invalid address - error %d\n", errno);
			close_p2p(session);
			free(header);
			return -1;
		}

		if (connect_p2p(session, port) < 0) {
			close_p2p(session);
			free(header);
			return -1;
		}

		// connection info
		char host_buff[256];
		struct hostent *host_entry;
		int host_name;

		// retrieve host information for header data
		if (gethostname(host_buff, sizeof(host_buff)) == -1 || (host_entry = gethostbyname(host_buff)) == NULL) {
			fprintf(stderr, "Unable to retrieve host information\n", errno);
			close_p2p(session);
			free(header);
			return -1;
		}

		// set header informaton
		inet_aton(host_entry->h_addr_list[0], &header->local_addr);
		header->port = port;
		// send header
		if (nbytes = send(session->client_socket, header, sizeof(p2p_header), 0) < 0) {
			fprintf(stderr, "Unable to send data - error %d\n", errno);
			close_p2p(session);
			free(header);
		}

		// accept incoming connection
		if (accept_p2p(session, port) < 0) {
			close_p2p(session);
			free(header);
			return -1;
		}
	}
	// send/recieve data from connetion
	transfer_data(session);

	close_p2p(session);
	free(header);
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
