#include "p2p.h"
#include "p2p_chat.h"

char **get_addrs(char *addr_list, int *n); // interprets list of addresses from command line
int *get_ports(char *port_list, int *n); // interprets list of ports from command line
void print_usage(); // displays usage options
void *manage_chat(void *arg); // manages client side connections

int main(int argc, char **argv) {

	static char *options = "a:hln:p:t:cfr";

	sconn = cconn = 0;
	int connect = 0, listen = 0;
	int opt, nbytes;
	char *addr_list = NULL;
	char *local_port_list = NULL, *target_port_list = NULL;

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
				sconn = sconn = atoi(optarg);
				break;
			case 'p':
				local_port_list = optarg;
				break;
			case 't':
				target_port_list = optarg;
				break;
			case '?':
				fprintf(stderr, "Improper usage, use \'-h\' for help\n");
				return -1;
			default:
				// getopt error, displays own error message
				return -1;
		}
	}

	int *local_ports = NULL;
	int *target_ports = NULL;

	// local ports
	if (local_port_list == NULL && listen) { // default setting
		if (sconn == 0)
			sconn = 2; // default # of connections is 2
		local_ports = calloc(sconn, sizeof(int));
		for(int i = 0; i < sconn; ++i)
			local_ports[i] = 18 + i;
	}
	else if (listen) // ports are specified
		local_ports = get_ports(local_port_list, &sconn);

	// target ports
	if (target_port_list == NULL && connect) { // default setting
		if (cconn == 0)
			cconn = 2; // default # of connections is 2
		target_ports = calloc(cconn, sizeof(int));
		for(int i = 0; i < cconn; ++i)
			local_ports[i] = 18 + i;
	}
	else if (connect) // ports are specified
		target_ports = get_ports(target_port_list, &cconn);

	// network options not specified, program must listen and/or connect
	if (!listen && !connect) {
		fprintf(stderr, "Improper usage, use \'-l\' and/or \'-a\' to connect\ntype \'-h\' for help\n");
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
	session = (p2p_struct**)calloc(cconn+sconn, sizeof(p2p_struct*));
	for (int i = 0; i < cconn; ++i)
		session[i] = init_p2p(target_ports[i]);
	for (int i = 0; i < sconn; ++i)
		session[i+cconn] = init_p2p(local_ports[i]);

	// multithreading, connecting to multiple addresses
	pthread_t cconnect[cconn];
	char **addrs = NULL;
	if (connect) {
		int n;
		addrs = get_addrs(addr_list, &n);
		if (n != cconn) {
			fprintf(stderr, "Address list doesn't match ports\ntype \'-h\' for help\n");
			for(int i = 0; i < n; ++i) {
				free(addrs[i]);
			}
			free(addrs);
			return -1;
		}
		for (int i = 0; i < cconn; ++i) {
			strcpy(session[i]->ip, addrs[i]);
			if (inet_pton(AF_INET, session[i]->ip, &session[i]->addr.sin_addr) <= 0) {
				fprintf(stderr, "[!] Invalid address \'%s\' - error %d\n", session[i]->ip, errno);
			} else {
				connect_p2p(i);
			}
		}
	}

	// multithreading, listening on multiple ports
	pthread_t slisten[sconn];
	if (listen) {
		for (int i = 0; i < sconn; ++i) {
			pthread_create(&slisten[i], NULL, accept_p2p, session[i]);
		}
	}

	// Chat Room
	fprintf(stdout, "Chat Room, type 'X' to exit:\n");

	// manages both server and client side connections
	pthread_t manage;
	pthread_create(&manage, NULL, manage_chat, NULL);

	// sends data to all connections
	send_data(session);

	// memory deallocation
	if (local_ports != NULL) free(local_ports);
	if (target_ports != NULL) free(target_ports);
	if (addrs != NULL) {
		for(int i = 0; i < cconn; ++i)
			free(addrs[i]);
		free(addrs);
	}
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
	fprintf(stdout, "usage: ./session <options>\n\n");
	fprintf(stdout, "network options:\n");
	fprintf(stdout, "\t-a a1,a2,...,an : connect to target addresses\n");
	fprintf(stdout, "\t-h              : display help options\n");
	fprintf(stdout, "\t-l              : listen for incoming connections\n");
	fprintf(stdout, "\t-n #            : maximum number of incoming connections\n");
	fprintf(stdout, "\t-p p1,p2,...,pn : local ports for accepting connections\n");
	fprintf(stdout, "\t-t p1,p2,...,pn : target ports for connecting\n");
	/* 
	fprintf(stdout, "interface options:\n");
	fprintf(stdout, "\t-c              : chat room\n");
	fprintf(stdout, "\t-f              : file transfer\n");
	fprintf(stdout, "\t-r              : remote command line interface\n");
	*/
	return;
}

/*
 * manages both client and server side connections
 * creates a thread to read incoming data if a connections is made
 */
void *manage_chat(void *arg) {

	pthread_t read[cconn+sconn];
	int flag = 1;

	while (flag) {
		for (int i = 0, n = cconn+sconn; i < n; ++i) {
			flag = 0;
			// if connection is made, read incoming data
			if (session[i]->connected == 1 && session[i]->active == 0) {
				session[i]->active = 1;
				pthread_create(&read[i], NULL, read_data, session[i]);
			}

			// if atleast one connection is still maintained, continue
			// exit otherwise
			if (!(session[i]->connected == 0 && session[i]->active == 1))
				flag = 1;
		}
	}
	return NULL;
}