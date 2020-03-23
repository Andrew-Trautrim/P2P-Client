#ifndef P2P_NETWORK_H
#define P2P_NETWORK_H

#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <arpa/inet.h>
//#include <errno.h>
//#include <ifaddrs.h>
//#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

namespace p2p {

    // struct for holding a server
    struct server {
        unsigned short port; // main port for listening
        int socket; // socket to accept new connections
        std::vector<int> accepted; // vector holding established connections
        struct sockaddr_in address;
    };

    // struct for holding a single client side connections
    struct client {
        unsigned short port; // port to connect to
        int socket; // local endpoit for communication
        struct sockaddr_in address;
    };

    // structure for creating/manipulating a peer to peer network
    class network {
    private:

        server *serv; // holds the server side connection
        std::vector<client*> clients; // holds all client side connections

        char fin[5] = "exit";
        int max_connections; // max number of incoming connection a server can hold

        bool accept_incoming();
        void send_outgoing();
        void read_incoming(int socket);

    public:

        // constructor
        // default number of connections per server is 1
        network(int max = 1) {
            max_connections = max;
            serv = NULL;
        }
        
        void init_server(unsigned short port = 22);
        void init_client(std::string address, unsigned short port = 22);
        void chat();
    };
}

#endif // P2P_NETWORK_H