
#include "p2p_network.h"

using namespace std;

void usage();

int main(int argc, char **argv) {

	static char *options = "a:hln:p:t:cfr";
    int n = 1, opt;
    bool listen = false, connect = false;

	// command line argument(s)
	while((opt = getopt(argc, argv, options)) != -1) {
		switch (opt) {
			case 'a':
				connect = true;
				break;
			case 'h':
				usage();
				return 1;
			case 'l':
				listen = true;
				break;
			case 'n':
				n = atoi(optarg);
				break;
			case '?':
				cerr << "Improper usage, use \'-h\' for help." << endl;
				return -1;
			default:
				// getopt error, displays own error message
				return -1;
		}
	}

    if (!connect && !listen) {
        cerr << "Nothing to do ¯\\_(ツ)_/¯" << endl << "Use \'-h\' for help." << endl;
        return -1;
    }

    p2p::network net(n);

    if (connect) {
        net.init_client("127.0.0.1");
    }

    if (listen) {
        net.init_server();
    }

    net.chat();

    return 0;
}

void usage() {
	cout << "usage: session <options>" << endl << endl;
	cout << "network options:" << endl;
	cout << "\t-a a1,a2,...,an : connect to target addresses" << endl;
	cout << "\t-h              : display help options" << endl;
	cout << "\t-l p            : listen for incoming connections on port p" << endl;
	cout << "\t-n #            : maximum number of incoming connections" << endl;
	cout << "\t-p p1,p2,...,pn : target ports for connecting" << endl;
	return;
}