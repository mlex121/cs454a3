#include "sender.h"

#include <iostream>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

using namespace std;

NetworkSender::NetworkSender(char dest_hostname[MAX_HOSTNAME_LEN], char dest_port[MAX_PORT_LEN]) :
    sock_fd(-1)
{
    strncpy(this->dest_hostname,    dest_hostname,  MAX_HOSTNAME_LEN);
    strncpy(this->dest_port,        dest_port,      MAX_PORT_LEN);

    struct addrinfo *servinfo = NULL;
    struct addrinfo hints = {};

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int ret = getaddrinfo(dest_hostname, dest_port, &hints, &servinfo);

    if (ret != 0) {
        throw NETWORK_ERROR;
    }

    struct addrinfo *p = NULL;

    for (p = servinfo; p != NULL; p = p->ai_next) {
        sock_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

        if (sock_fd == -1) {
            continue;
        }

        if (connect(sock_fd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sock_fd);
            continue;
        }

        break;
    }

    if (p == NULL) {
        throw NETWORK_ERROR;
    }

    /*
    inet_ntop(
        p->ai_family,
        get_in_addr((struct sockaddr *)p->ai_addr),
        remoteIP,
        sizeof(remoteIP)
    );

    //cout << "client: connected to " << remoteIP << " on port " << port << endl;
    */

    freeaddrinfo(servinfo);
}

void NetworkSender::send_message(message *m) {
    int size = ((int *)m->buf)[0];
    int offset = 0;
    int ret;

    while (offset != size) {
        if ((ret = send(sock_fd, m->buf + offset, size - offset, 0)) == -1) {
            throw NETWORK_ERROR;
        }

        offset += ret;
    }

    //Free the message
    delete[] m->buf;
    delete m;
}

message* NetworkSender::receive_reply() {
    int size; 
    int offset = 0;
    char buf[MAX_SEND_SIZE];

    message *m = NULL;

    if ((size = recv(sock_fd, buf, METADATA_LEN, 0)) > 0) {
        int message_length = *((int *)buf);

        m = new message;
        m->buf = new char[message_length];

        do {
            memcpy(m->buf + offset, buf, size);
            offset += size;
        }
        while (
            offset != message_length &&
            (size = recv(sock_fd, buf, min(message_length - offset, MAX_SEND_SIZE), 0)) > 0
        );

        if (message_length == offset) {
            return m;
        }
    }

    throw NETWORK_ERROR;
    return NULL;
}

