#include <iostream>
#include <string>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080

using namespace std;

int main(int argc, char **argv) {

	if (argc != 2) {
		cout << "Usage: " << argv[0] << " <IP Address>" << endl;
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
		cerr << "Unable to create client side socket" << endl;
		return -1;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);

	// converts IPv4 and IPv6 addresses from text to binary form
	if (inet_pton(AF_INET, addr, &server_addr.sin_addr) <= 0) {
		cerr << "Invalid server address" << endl;
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
	struct sockaddr_in client_addr;
	
	// socket creation
	if (server_socket = socket(AF_INET, SOCK_STREAM, 0) == 0) {
		cerr << "Unable to create server side socket" << endl;
		return -1;
	}

	client_addr.sin_family = AF_INET;
	client_addr.sin_addr.s_addr = INADDR_ANY;
	client_addr.sin_port = htons(PORT);

	// bind socket to port 8080
	if (bind(server_socket, (struct sockaddr*)&client_addr, sizeof(client_addr)) < 0) {
		cerr << "Failure to bind socket to port " << PORT << endl;
		return -1;
	}

	// listens for incoming connections
	// maximum of 1 pending connections
	if (listen(server_socket, 1) < 0) {
		cerr << "Unable to listen on port " << PORT << endl;
	}

	// server side connection
	if (conn_socket = accept(server_socket, (struct sockaddr*)&client_addr, (socklen_t*)sizeof(client_addr)) < 0) {
		cerr << "Failure to accept connection" << endl;
		return -1;
	}

	// client side connection
	if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		cerr << "Connection failure" << endl;
		return -1;
	}

	// input sequence
	char input[2048];
	do {
		cout << ">";
		cin >> input;
		send(client_socket, input, char_traits<char>::length(input), 0);
	} while (input != "exit");
}
