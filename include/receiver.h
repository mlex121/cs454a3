#ifndef RECEIVER_H
#define RECEIVER_H

#include <unordered_map>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "common_defs.h"

#define MAX_CONNECTIONS 5

struct message_assembly {
    // The current end of the buffer, where we write to
    int offset;

    char *buf;
};

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

    int safe_receive(int fd, char *buf, int len, int &size);
    void handle_client_data(int fd);
    void cleanup_fd(int fd);
    void handle_set_fd(int fd);

    protected:
        std::unordered_map<int, message_assembly> received_messages;

        char hostname[MAX_HOSTNAME_LEN];
        char port[MAX_PORT_LEN];
        struct sockaddr_in addr_info;

        virtual void extra_setup();
        virtual void process_message(int fd) = 0;

    public:
        NetworkReceiver();
        void run();
};

#endif
