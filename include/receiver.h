#ifndef RECEIVER_H
#define RECEIVER_H

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "common_defs.h"

//FIXME what is this supposed to be?
#define MAX_CONNECTIONS 100

class NetworkReceiver {
    int listen_fd;
    int new_fd;
    int max_fd;

    struct sockaddr_in receiver_addr;
    struct sockaddr_storage sender_addr;

    fd_set master_fds;
    fd_set current_fds;

    socklen_t sock_len;
    socklen_t sender_sock_len;

    message *m;

    protected:
        char hostname[MAX_HOSTNAME_LEN];
        struct sockaddr_in addr_info;

        virtual void handle_set_fd(int fd);
        virtual void extra_setup();

    public:
        NetworkReceiver();
        void run();
};

#endif
