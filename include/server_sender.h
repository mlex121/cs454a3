#ifndef SERVER_SENDER_H
#define SERVER_SENDER_H

#include <list>

#include <netdb.h>
#include <semaphore.h>
#include <stdlib.h>
#include <sys/wait.h>

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

    char *hostname;
    char *port;

    public:
        ServerSender();
        int rpcRegister(char *name, int *argTypes, skeleton f);
        void run();
        static void *dispatch(void *arg);
};

#endif
