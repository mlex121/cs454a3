#include "sender.h"

#include <iostream>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

using namespace std;

NetworkSender::NetworkSender(char dest_hostname[MAX_HOSTNAME_LEN], char dest_port[MAX_PORT_LEN]) :
    sock_fd(-1)
{
    strncpy(this->dest_hostname,    dest_hostname,  MAX_HOSTNAME_LEN);
    strncpy(this->dest_port,        dest_port,      MAX_PORT_LEN);

    int ret = -1;
    struct addrinfo *servinfo = NULL;
    struct addrinfo hints = {};

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    ret = getaddrinfo(dest_hostname, dest_port, &hints, &servinfo);

    if (ret != 0) {
        cerr << "getaddrinfo: " << gai_strerror(ret) << endl;
        exit(1);
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
        cerr << "client: failed to connect" << endl;
        exit(1);
    }

    /*
    inet_ntop(
        p->ai_family,
        get_in_addr((struct sockaddr *)p->ai_addr),
        remoteIP,
        sizeof(remoteIP)
    );

    cout << "client: connected to " << remoteIP << " on port " << port << endl;
    */

    freeaddrinfo(servinfo);
}

int NetworkSender::send_message(message *m) {
    // FIXME do this in a loop
    if (send(sock_fd, m->buf, ((int *)m->buf)[0], 0) == -1) {
        perror("send");
        return -1;
    }

    return 0;
}
