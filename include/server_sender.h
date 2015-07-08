#ifndef SERVER_SENDER_H
#define SERVER_SENDER_H

#include <list>

#include <netdb.h>
#include <semaphore.h>
#include <stdlib.h>

#include "rpc.h"
#include "sender.h"

#define NUM_SERVER_ERRORS 100

enum SERVER_ERRORS {
  BINDER_NOT_FOUND = -1*NUM_SERVER_ERRORS, //all server errors should have negative codes
  UNITIALIZED_NETWORK_HANDLERS
};

enum SERVER_WARNINGS {
    ALREADY_INITIALIZED_NETWORK_HANDLERS = 1
};

class ServerSender : NetworkSender {
    std::list<std::string> client_data;

    sem_t read_avail;
    sem_t write_avail;

    char receiver_hostname[MAX_HOSTNAME_LEN];
    char receiver_port[MAX_PORT_LEN];

    public:
        ServerSender(char receiver_hostname[MAX_HOSTNAME_LEN], char receiver_port[MAX_PORT_LEN]);
        int rpcRegister(char *name, int *argTypes, skeleton f);
        int rpcExecute();
        void run();
        static void *dispatch(void *arg);
};

#endif
