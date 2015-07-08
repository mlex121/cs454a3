#ifndef SENDER_H
#define SENDER_H

#include <netdb.h>

#include "common_defs.h"

class NetworkSender {
    int sock_fd;

    char dest_hostname[MAX_HOSTNAME_LEN];
    char dest_port[MAX_PORT_LEN];

    char remoteIP[INET6_ADDRSTRLEN];

protected:
    int send_message(message *m);

public:
    NetworkSender(char dest_hostname[MAX_HOSTNAME_LEN], char dest_port[MAX_PORT_LEN]);
};

#endif // SENDER_H
