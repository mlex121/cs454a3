#include "common_defs.h"
#include "message.h"
#include "server_sender.h"

#include <iostream>

#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
//#include <sys/wait.h>

using namespace std;

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

/*
void* ServerSender::dispatch(void *arg) {
  ServerSender *c = (ServerSender *)arg;

  int size;
  char buf[MAX_DATA_LEN];
  message *m;

  string s;

  while(true) {
    //Lock the client data structure while we grab the first item
    sem_wait(&c->read_avail);
    s = c->client_data.front();
    c->client_data.pop_front();
    sem_post(&c->write_avail);

    //Send data to server
    if (send(c->sock_fd, s.c_str(), s.length(), 0) == -1) {
        perror("send");
    }

    //Receive server reply
    if ((size = recv(c->sock_fd, buf, MAX_DATA_LEN, 0)) == -1) {
        perror("recv");
        exit(1);
    }

    m = (message*)(buf);

    //cout << "Length: " << m->length << endl;
    cout << "Server: " << m->s << endl;

    //Wait at least two seconds before attempting to send more data
    sleep(2);
  }

  pthread_exit(0);
}
*/

ServerSender::ServerSender(char receiver_hostname[MAX_HOSTNAME_LEN], char receiver_port[MAX_PORT_LEN]) {

    strncpy(this->receiver_hostname,    receiver_hostname,  MAX_HOSTNAME_LEN);
    strncpy(this->receiver_port,        receiver_port,      MAX_PORT_LEN);

    strncpy(dest_hostname,  getenv("BINDER_ADDRESS"),    MAX_HOSTNAME_LEN);
    strncpy(dest_port,      getenv("BINDER_PORT"),       MAX_PORT_LEN);

    memset(&hints, 0, sizeof hints);
    hints.ai_family =   AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(dest_hostname, dest_port, &hints, &servinfo)) != 0) {
        cerr << "getaddrinfo: " << gai_strerror(rv) << endl;
        exit(1);
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
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
        throw BINDER_NOT_FOUND;
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

    sem_init(&read_avail, 0, 0);
    sem_init(&write_avail, 0, 1);
}

/*
void ServerSender::run() {
    pthread_t worker;
    string s;

    int ret = pthread_create(&worker, NULL, &ServerSender::dispatch, (void *)(this));
    if (ret) {
        cout << "Error: Unable to spawn worker thread." << endl;
    }

    cout << "Working" << endl;

    while (getline(cin, s)) {
        if (s.length()) {
            sem_wait(&write_avail);
            client_data.push_back(s);
            sem_post(&read_avail);
        }
    }
}
*/

int ServerSender::send_message(message *m) {
    //cout << "Arg length is: " << get_args_len(argTypes) << endl;
    //cout << "Message hostname is: " << m->buf + 8 << endl;
    cout << "Message length is: " << *((int *)m->buf) << endl;

    //Send data to server
    // FIXME do this in a loop
    if (send(sock_fd, m->buf, *((int *)m->buf), 0) == -1) {
        perror("send");
    }

    //FIXME obviously
    return 0;
}

int ServerSender::rpcRegister(char *name, int *argTypes, skeleton f) {
    message *m = get_register_request(receiver_hostname, receiver_port, name, argTypes);

    send_message(m);

    /*
    //Receive server reply
    if ((size = recv(c->sock_fd, buf, MAX_DATA_LEN, 0)) == -1) {
        perror("recv");
        exit(1);
    }
    */

    return 0;
}
