#ifndef CLIENT_SENDER_H
#define CLIENT_SENDER_H

#include <list>

#include <netdb.h>
#include <semaphore.h>
#include <stdlib.h>

#include "common_defs.h"
#include "rpc.h"
#include "sender.h"

class ClientSender : NetworkSender {
        FunctionLocations function_locations;
        int execute(const char *hostname, const char *port, char *name, int *argTypes, void **args);

    public:
        ClientSender();
        ClientSender(const char* hostname, const char* port);
        void rpcCall(char *name, int *argTypes, void ** args);
        void rpcCacheCall(char *name, int *argTypes, void **args);
        void rpcTerminate();
};

#endif
