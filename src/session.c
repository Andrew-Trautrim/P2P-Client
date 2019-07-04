#include "p2p.h"

void print_usage();

int main(int argc, char **argv) {

	static char *options = "a:hlp:t:";

	int opt, nbytes;
	int listen = 0;
	int connect = 0;
	int target_port = 8080;
	int local_port = 8080;
	char *addr = NULL;
	p2p_header *header;
	p2p_struct *client;
	p2p_struct *server;

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

	// creating connection
	header = (p2p_header*)calloc(1, sizeof(p2p_header));
	if (listen) {

		server = init_p2p();

		if (accept_p2p(server, local_port, server->nconn) < 0) {
			close_p2p(server);
			free(header);
			return -1;
		}
		
		// read incoming connection data to establish client side connection
		fprintf(stderr, "Reading incoming data...");
		if (nbytes = read(server->connection[server->nconn], header, sizeof(p2p_header)) < 0) {
			fprintf(stderr, "\nUnable to read incoming data - error %d\n", errno);
			close_p2p(server);
			free(header);
			return -1;
		}
		fprintf(stderr, "recieved\n");

		server->nconn++;
	} 
	
	if (connect) {

		client = init_p2p();

		if (inet_pton(AF_INET, addr, &client->addr[0].sin_addr) <= 0) {
			fprintf(stderr, "Invalid address - error %d\n", errno);
			close_p2p(client);
			free(header);
			return -1;
		}

		if (connect_p2p(client, target_port, client->nconn) < 0) {
			close_p2p(client);
			free(header);
			return -1;
		}

		// connection info
		char host_buff[256];
		struct hostent *host_entry;
		int host_name;

		// retrieve host information for header data
		if (gethostname(host_buff, sizeof(host_buff)) == -1 || (host_entry = gethostbyname(host_buff)) == NULL) {
			fprintf(stderr, "Unable to retrieve host information - error %d\n", errno);
			close_p2p(client);
			free(header);
			return -1;
		}

		// set header informaton
		inet_aton(host_entry->h_addr_list[0], &header->local_addr);
		header->port = local_port;
		// send header
		fprintf(stderr, "Sending local information\n");
		if (nbytes = send(client->socket[client->nconn], header, sizeof(p2p_header), 0) < 0) {
			fprintf(stderr, "Unable to send data - error %d\n", errno);
			close_p2p(client);
			free(header);
		}

		client->nconn++;
	}
	
	fprintf(stdout, "Session (type \'X\' to exit): \n");

	// multithreading
	pthread_t client_send;
	pthread_t client_recieve;
	pthread_t server_send;
	pthread_t server_recieve;
	if (listen) {
		pthread_create(&server_send, NULL, send_data, server);
		pthread_create(&server_recieve, NULL, recieve_data, server);
	}
	if (connect) {
		pthread_create(&client_send, NULL, send_data, client);
		pthread_create(&client_recieve, NULL, recieve_data, client);
	}

	close_p2p(client);
	close_p2p(server);
	free(header);
	return 0;
}

/* prints help message for proper usage */
void print_usage() {
	fprintf(stdout, "usage: ./session <options>\n\n");
	fprintf(stdout, "options:\n");
	fprintf(stdout, "	-a address : connect to target address\n");
	fprintf(stdout, "	-h	   : display help options\n");
	fprintf(stdout, "	-l	   : listen for incoming connections\n");
	fprintf(stdout, "	-p port	   : local port for accepting connections\n");
	fprintf(stdout, "	-t port    : target port for connecting\n");
	return;
}
