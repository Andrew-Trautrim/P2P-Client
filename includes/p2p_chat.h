#include "p2p.h"

int send_data(p2p_struct **server, p2p_struct *client); // sends data to all connections

void *read_client(void *arg); // reads data sent from clients
void *read_server(void *arg); // reads data sent from the server connection