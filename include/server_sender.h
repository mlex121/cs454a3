#ifndef SERVER_SENDER_H
#define SERVER_SENDER_H

#include <list>

#include <netdb.h>
#include <stdlib.h>

#include "rpc.h"
#include "sender.h"

class ServerSender : NetworkSender {

    char receiver_hostname[MAX_HOSTNAME_LEN];
    char receiver_port[MAX_PORT_LEN];

    static void *receive_termination(void *arg);

    public:
        ServerSender(char receiver_hostname[MAX_HOSTNAME_LEN], char receiver_port[MAX_PORT_LEN]);
        void rpcRegister(char *name, int *argTypes, skeleton f);
        void await_termination();
};

#endif
