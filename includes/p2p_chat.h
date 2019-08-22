#ifndef P2P_CHAT_H
#define P2P_CHAT_H

#include "p2p.h"

#define MSG_LEN 64 // max length of recieved message

typedef struct {
    char sender[32];
    char origin[32];
    char msg[MSG_LEN];
} message;

message recieved_message;

int compare(message msg1, message msg2); // compares two messages
int send_data(p2p_struct **session); // sends data to all connections

void *broadcast_data(void *arg); // broadcasts recieved message to all connections
void *read_data(void *arg); // reads incoming data sent from client and/or server

#endif