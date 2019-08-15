#ifndef P2P_CHAT_H
#define P2P_CHAT_H

#include "p2p.h"

int send_data(p2p_struct **server, p2p_struct *client); // sends data to all connections
void *read_data(void *arg); // reads incoming data sent from client and/or server

#endif