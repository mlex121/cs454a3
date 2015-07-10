#ifndef CLIENT_SENDER_H
#define CLIENT_SENDER_H

#include <list>

#include <netdb.h>
#include <semaphore.h>
#include <stdlib.h>

#include "rpc.h"
#include "sender.h"

class ClientSender : NetworkSender {

    public:
        ClientSender();
        ClientSender(char* hostname, char* port);
        int rpcCall(char *name, int *argTypes, void ** args);
};

#endif
