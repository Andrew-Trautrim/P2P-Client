
#include "p2p_network.h"

// listens for incoming connections
bool p2p::network::accept_incoming() {

    if (serv->accepted.size() >= max_connections) {
        std::cerr << "[!] Maximum number of server side connections created." << std::endl;
        return false;
    }

    // accept incoming connections
    int new_socket;
    int addrlen = sizeof(serv->address);
    if ((new_socket = accept(serv->socket, (struct sockaddr*)&(serv->address), (socklen_t*)&addrlen)) < 0) {
        std::cerr << "[!] Failed to accept incoming connection." << std::endl;
        return false;
    }

    std::cout << "Connection accepted on port " << serv->port << "." << std::endl;
    serv->accepted.push_back(new_socket);

    return true;
}

void p2p::network::send_outgoing() {
    
    int nbytes, len;
    size_t buffsize = 1024;
    char *buffer = (char*)calloc(buffsize, sizeof(char));

    while((len = getline(&buffer, &buffsize, stdin)) > 0) {
        buffer[len-1] = '\0'; // remove newline

        if (serv != NULL) {
            // send buffer to all server connections
            for (auto socket : serv->accepted) {
                nbytes = send(socket, buffer, sizeof(char) * len, 0);
                if (nbytes < 0)
                    close(socket);
            }
        }

        // send buffer to all client connections
        for (auto conn : clients) {
            nbytes = send(conn->socket, buffer, len, 0);
            if (nbytes < 0)
                close(conn->socket);
        }

        if (strcmp(buffer,fin) == 0)
            break;
    }
    free(buffer);
}

void p2p::network::read_incoming(int socket) {

    int nbytes;
    char buffer[1024];

    do {
        nbytes = read(socket, buffer, 1024);
        if (strcmp(buffer, fin) == 0) {
            std::cout << "[!] Connection terminated." << std::endl;
            break;
        }
        std::cout << "[*] " << buffer << std::endl;
    } while (nbytes > 0);

    close(socket);
}

// initiates a server side connection, default port is 22
void p2p::network::init_server(unsigned short port) {

    serv = new server;
    serv->port = port;

    // create socket
    if ((serv->socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "[!] Socket failed." << std::endl;
        return;
    }

    // set parameters
    serv->address.sin_family = AF_INET;
    serv->address.sin_addr.s_addr = INADDR_ANY;
    serv->address.sin_port = htons(serv->port);

    // bind socket to port
    if (bind(serv->socket, (struct sockaddr*)&(serv->address), sizeof(serv->address)) < 0) {
        std::cerr << "[!] Failed to bind socket to port " << serv->port << "." << std::endl;
        return;
    }

    // listen for incoming connections
    if (listen(serv->socket, max_connections) < 0) {
        std::cerr << "[!] Unable to listen on port " << serv->port << "." << std::endl;
        return;
    }

    std::cout << "Listening on port " << serv->port << "..." << std::endl;
}

// initiates a client side connection, default port is 22
void p2p::network::init_client(std::string address, unsigned short port) {

    client *c = new client;
    c->port = port;

    // creates client side socket
    if ((c->socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "[!] Unable to create client side socket." << std::endl;
        return;
    }

    // connection settings
    c->address.sin_family = AF_INET;
    c->address.sin_port = htons(c->port);

    // client side connection
    if (connect(c->socket, (struct sockaddr*)&(c->address), sizeof(c->address)) == -1) {
        std::cerr << "[!] Unable to connect to " << address << "." << std::endl;
        return;
    }

    std::cout << "Connected to " << address << " on port " << port << "." << std::endl;
    clients.push_back(c);
}

// main functionality for group chat
void p2p::network::chat() {

    std::cout << "Chat room, type \'exit\' to end." << std::endl;

    std::thread outgoing_data(&network::send_outgoing, this);

    // each client side connections gets a thread for reading incoming data
    std::vector<std::thread> client_incoming;
    for (auto c : clients) {

        // create thread for reading data
        std::thread t(&network::read_incoming, this, c->socket);
        client_incoming.push_back(std::move(t));
    }

    // accept incoming connections until max number of incoming connections is reached, or an error occurs
    std::vector<std::thread> server_incoming;
    if (serv != NULL) {
        while (accept_incoming()) {

            // create thread for reading data
            std::thread t(&network::read_incoming, this, serv->accepted[serv->accepted.size() - 1]);
            server_incoming.push_back(std::move(t));
        }
    }

    // wait for each thread to terminate
    for (auto &t : client_incoming)
        t.join();
    for (auto &t : server_incoming)
        t.join();
    outgoing_data.join();
}