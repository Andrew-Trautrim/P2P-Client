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
		cout << "Usage: " << argv[0] << "<IP Address>" << endl;
		return -1;
	}

	char *addr = argv[1];
	int sock;
	struct sockaddr_in address;

	// socket creation
	if (sock = socket(AF_INET, SOCK_STREAM, 0) == 0) {
		cerr << "Unable to create socket" << endl;
		return -1;
	}

	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);

	// converts IPv4 and IPv6 addresses from text to binary form
	if (inet_pton(AF_INET, addr, &address.sin_addr) <= 0) {
		cerr << "Invalid address" << endl;
		return -1;
	}
	
	// server connection
	if (connect(sock, (struct sockaddr *)&address, sizeof(address)) < 0) {
		cerr << "Unable to connect" << endl;
		return -1;
	}

	// input sequence
	cout << "" << endl;
	char input[2048];
	do {
		cout << ">";
		cin >> input;
		send(sock, input, char_traits<char>::length(input), 0);
	} while (input != "exit");
}
