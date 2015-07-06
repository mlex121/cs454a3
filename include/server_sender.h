#ifndef SERVER_SENDER_H
#define SERVER_SENDER_H

#include <list>

#include <netdb.h>
#include <semaphore.h>
#include <stdlib.h>

#include "rpc.h"

#define NUM_SERVER_ERRORS 100

enum SERVER_ERRORS {
  BINDER_NOT_FOUND = -1*NUM_SERVER_ERRORS, //all server errors should have negative codes
  UNITIALIZED_NETWORK_HANDLERS
};

enum SERVER_WARNINGS {
    ALREADY_INITIALIZED_NETWORK_HANDLERS = 1
};

class ServerSender {
    int sock_fd;

    std::list<std::string> client_data;

    sem_t read_avail;
    sem_t write_avail;

    struct addrinfo hints, *servinfo, *p;
    int rv;
    char remoteIP[INET6_ADDRSTRLEN];

    char receiver_hostname[MAX_HOSTNAME_LEN];
    char receiver_port[MAX_PORT_LEN];

    char dest_hostname[MAX_HOSTNAME_LEN];
    char dest_port[MAX_PORT_LEN];

    int send_message(message *m);

    public:
        ServerSender(char receiver_hostname[MAX_HOSTNAME_LEN], char receiver_port[MAX_PORT_LEN]);
        int rpcRegister(char *name, int *argTypes, skeleton f);
        int rpcExecute();
        void run();
        static void *dispatch(void *arg);
};

#endif
