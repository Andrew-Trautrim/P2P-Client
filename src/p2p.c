#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080

int main(int argc, char **argv) {

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <IP Address>\n", argv[0]);
		return -1;
	}

	/* CLIENT SIDE CONNECTION SETUP 
	 * socket
	 * connect
	 * send / recieve
	 */
	char *addr = argv[1];
	int client_socket;
	struct sockaddr_in server_addr;

	// socket creation
	if (client_socket = socket(AF_INET, SOCK_STREAM, 0) == 0) {
		fprintf(stderr, "Unable to create client side socket\n");
		return -1;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);

	// converts IPv4 and IPv6 addresses from text to binary form
	if (inet_pton(AF_INET, addr, &server_addr.sin_addr) <= 0) {
		fprintf(stderr, "Invalid IP address\n");
		return -1;
	}
	
	/* SERVER SIDE CONNECTION SETUP 
	 * socket
	 * setsockopt
	 * bind
	 * listen
	 * accept
	 * send / recieve
	 */
	int server_socket, conn_socket;
	int opt = 1;
	struct sockaddr_in client_addr;
	
	// socket creation
	if (server_socket = socket(AF_INET, SOCK_STREAM, 0) == 0) {
		fprintf(stderr, "Unable to create server side socket\n");
		return -1;
	}

	client_addr.sin_family = AF_INET;
	client_addr.sin_addr.s_addr = INADDR_ANY;
	client_addr.sin_port = htons(PORT);

	// bind socket to port 8080
	if (bind(server_socket, (struct sockaddr*)&client_addr, sizeof(client_addr)) == -1) {
		fprintf(stderr, "Failure to bind socket to port %d\n", PORT);
		return -1;
	}

	// listens for incoming connections
	// maximum of 1 pending connections
	if (listen(server_socket, 1) == -1) {
		fprintf(stderr, "Unable to listen on port %d\n", PORT);
		return -1;
	}

	// server side connection
	if (conn_socket = accept(server_socket, (struct sockaddr*)&client_addr, (socklen_t*)sizeof(client_addr)) == -1) {
		fprintf(stderr, "Failure to accept connection");
		return -1;
	}

	// client side connection
	if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
		fprintf(stderr, "Connection failure");
		return -1;
	}

	// input sequence
	char input[2048];
	do {
		fprintf(stdin, ">");
		fscanf(stdin, "%s", input);
		send(client_socket, input, strlen(input), 0);
	} while (strcmp(input, "exit") != 0);
}
