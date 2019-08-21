#ifndef P2P_CHAT_H
#define P2P_CHAT_H

#include "p2p.h"

#define MSG_LEN 64 // max length of recieved message
char recieved_message[MSG_LEN];

int send_data(p2p_struct **session); // sends data to all connections

void *broadcast_data(void *arg); // broadcasts recieved message to all connections
void *read_data(void *arg); // reads incoming data sent from client and/or server

#endif