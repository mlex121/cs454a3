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

ServerSender::ServerSender(char receiver_hostname[MAX_HOSTNAME_LEN], char receiver_port[MAX_PORT_LEN]) :
    NetworkSender(getenv("BINDER_ADDRESS"), getenv("BINDER_PORT"))
{
    strncpy(this->receiver_hostname,    receiver_hostname,  MAX_HOSTNAME_LEN);
    strncpy(this->receiver_port,        receiver_port,      MAX_PORT_LEN);

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
