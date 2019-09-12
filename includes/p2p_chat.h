#ifndef P2P_CHAT_H
#define P2P_CHAT_H

#include "p2p.h"

typedef struct {
    char msg[1024];
    char origin[32];
} message;

int send_data(); // sends data to all connections
void *read_data(void *arg); // reads incoming data sent from client and/or server

#endif