#include <iostream>
#include <list>

#include <arpa/inet.h>
#include <assert.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "rpc.h"
#include "common_defs.h"
#include "message.h"

enum SERVER_ERRORS {
  BINDER_NOT_FOUND,
  UNITIALIZED_BINDER_NETWORK_HANDLER
};

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

using namespace std;

class ServerSender {
    int sock_fd;

    list<string> client_data;

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

ServerSender::ServerSender() {
    hostname    = getenv("SERVER_ADDRESS");
    port        = getenv("SERVER_PORT");

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(hostname, port, &hints, &servinfo)) != 0) {
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

void ServerSender::run() {
    pthread_t worker;
    string s;

    /*
    int ret = pthread_create(&worker, NULL, &ServerSender::dispatch, (void *)(this));
    if (ret) {
        cout << "Error: Unable to spawn worker thread." << endl;
    }
    */

    cout << "Working" << endl;

    while (getline(cin, s)) {
        if (s.length()) {
            sem_wait(&write_avail);
            client_data.push_back(s);
            sem_post(&read_avail);
        }
    }
}

int ServerSender::rpcRegister(char *name, int *argTypes, skeleton f) {
    message *m = get_register_request(hostname, port, name, argTypes);

    //cout << "Arg length is: " << get_args_len(argTypes) << endl;
    //cout << "Message hostname is: " << m->buf + 8 << endl;
    cout << "Message length is: " << *((int *)m->buf) << endl;

    //Send data to server
    // FIXME do this in a loop
    if (send(sock_fd, m->buf, *((int *)m->buf), 0) == -1) {
        perror("send");
    }

    /*
    //Receive server reply
    if ((size = recv(c->sock_fd, buf, MAX_DATA_LEN, 0)) == -1) {
        perror("recv");
        exit(1);
    }
    */

    return 0;
}

ServerSender *server_sender = NULL;

int rpcInit() {
    if (server_sender == NULL) {
        try {
            server_sender = new ServerSender();
        }
        catch (SERVER_ERRORS e) {
            return e;
        }
    }
    else {
      cerr << "rpcInit called more than once" << endl;
    }

    return 0;
}

int rpcRegister(char* name, int* argTypes, skeleton f) {
    if (server_sender == NULL) {
      throw UNITIALIZED_BINDER_NETWORK_HANDLER;
    }

    return server_sender->rpcRegister(name, argTypes, f);
}

int rpcExecute() {
    return 0;
}
