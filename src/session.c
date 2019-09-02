#include "p2p.h"
#include "p2p_chat.h"

char **get_addrs(char *addr_list, int *n); // interprets list of addresses from command line
int *get_ports(char *port_list, int *n); // interprets list of ports from command line
void print_usage(); // displays usage options
void *manage_client(void *arg); // manages client side connections
void *manage_server(void *arg); // manages connections in the background

int main(int argc, char **argv) {

	static char *options = "a:hln:p:t:cfr";

	sconn = 0; // default number of connections is 2
	cconn = 0;
	int connect = 0;
	int listen = 0;
	int use = 0;
	int opt, nbytes;
	char *addr_list = NULL;
	char *local_port_list = NULL;
	char *target_port_list = NULL;

	// command line argument(s)
	while((opt = getopt(argc, argv, options)) != -1) {
		switch (opt) {
			case 'a':
				connect = 1;
				addr_list = optarg;
				break;
			case 'h':
				print_usage();
				return 1;
			case 'l':
				listen = 1;
				break;
			case 'n':
				sconn = atoi(optarg);
				cconn = sconn;
				break;
			case 'p':
				local_port_list = optarg;
				break;
			case 't':
				target_port_list = optarg;
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

	int *local_ports;
	int *target_ports;

	// local ports
	if (local_port_list == NULL) { // default setting
		local_ports = calloc(sconn, sizeof(int));
		for(int i = 0; i < sconn; ++i)
			local_ports[i] = 18 + i;
	}
	else // ports are specified
		local_ports = get_ports(local_port_list, &sconn);

	// target ports
	if (target_port_list == NULL) { // default setting
		target_ports = calloc(cconn, sizeof(int));
		for(int i = 0; i < cconn; ++i)
			local_ports[i] = 18 + i;
	}
	else // ports are specified
		target_ports = get_ports(target_port_list, &cconn);

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

	// initiate structure
	p2p_struct *session[cconn+sconn];
	for (int i = 0; i < cconn; ++i)
		session[i] = init_p2p(target_ports[i]);
	for (int i = 0; i < sconn; ++i)
		session[i+cconn] = init_p2p(local_ports[i]);

	// multithreading, listening on multiple ports
	pthread_t slisten[sconn];
	if (listen) 
		for (int i = 0; i < sconn; ++i)
			pthread_create(&slisten[i], NULL, accept_p2p, session[i+cconn]);
	
	// multithreading, connecting to multiple addresses
	pthread_t cconnect[cconn];
	char **addrs;
	if (connect) {
		int n;
		addrs = get_addrs(addr_list, &n);
		if (n != cconn) {
			fprintf(stderr, "Address list doesn't match ports\ntype \'h\' for help\n");
			for(int i = 0; i < n; ++i) {
				free(addrs[i]);
			}
			free(addrs);
			return -1;
		}
		for (int i = 0; i < cconn; ++i) {
			strcpy(session[i]->ip, addrs[i]);
			if (inet_pton(AF_INET, session[i]->ip, &session[i]->addr.sin_addr) <= 0) {
				fprintf(stderr, "[!] Invalid address %s - error %d\n", session[i]->ip, errno);
			} else {
				pthread_create(&cconnect[i], NULL, connect_p2p, session[i]);
			}
		}
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

	
	free(local_ports);
	free(target_ports);
	for(int i = 0; i < cconn; ++i)
		free(addrs[i]);
	free(addrs);
	close_p2p(session);
	return 0;
}

char **get_addrs(char *addr_list, int *n) {
	int len = strlen(addr_list);
	int j = 0, start = 0, buffer = 2;
	int end;
	char **addrs = calloc(buffer, sizeof(char*));
	for(int i = 0; i < len; ++i) {
		if (j >= buffer) { // allocates size accordingly
			addrs = realloc(addrs, buffer * sizeof(char*));
			buffer += 2;
		}
		if (addr_list[i] == ',' || i+1 == len) {
			end = (addr_list[i] == ',') ? i : i+1;
			char temp[end-start+1];
			int c = 0;
			while (start < end) {
				temp[c++] = addr_list[start++];
			}
			temp[c] = '\0';
			start++;
			addrs[j] = calloc(strlen(temp), sizeof(char));
			strcpy(addrs[j++], temp);
		}
	}
	(*n) = j;
	return addrs;
}

int *get_ports(char *port_list, int *n) {
	int len = strlen(port_list);
	int j = 0, start = 0, buffer = 2;
	int end;
	int *ports = calloc(buffer, sizeof(int));
	for(int i = 0; i < len; ++i) {
		if (j >= buffer) { // allocates size accordingly
			ports = realloc(ports, buffer * sizeof(int));
			buffer += 2;
		}
		if (port_list[i] == ',' || i+1 == len) {
			end = (port_list[i] == ',') ? i : i+1;
			char temp[end-start+1];
			int c = 0;
			while (start < end) {
				temp[c++] = port_list[start++];
			}
			temp[c] = '\0';
			start++;
			ports[j++] = atoi(temp);
		}
	}
	(*n) = j;
	return ports;
}

/* prints help message for proper usage */
void print_usage() {
	fprintf(stdout, "usage: ./session <options>\n");
	fprintf(stdout, "network options:\n");
	fprintf(stdout, "	-a address : connect to target address\n");
	fprintf(stdout, "	-h	   : display help options\n");
	fprintf(stdout, "	-l	   : listen for incoming connections\n");
	fprintf(stdout, "	-n max #   : maximum number of incoming connections\n");
	fprintf(stdout, "	-p port-1,port-2,...,port-n	   : local ports for accepting connections\n");
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
	pthread_t read[sconn];
	int flag = 1;

	while (flag) {
		for (int i = 0; i < sconn; ++i) {
			flag = 0;
			// if connection is made, read incoming data
			if (session[i+cconn]->connected == 1 && session[i+cconn]->active == 0) {
				session[i+cconn]->active = 1;
				pthread_create(&read[i], NULL, read_data, session[i+cconn]);
			}

			// if atleast one connection is still maintained, continue
			// exit otherwise
			if (!(session[i+1]->connected == 0 && session[i+1]->active == 1))
				flag = 1;
		}
	}
	return NULL;
}