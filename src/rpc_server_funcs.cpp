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

class BinderNetworkHandler {
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
        BinderNetworkHandler();
        int rpcRegister(char *name, int *argTypes, skeleton f);
        void run();
        static void *dispatch(void *arg);
};

/*
void* BinderNetworkHandler::dispatch(void *arg) {
  BinderNetworkHandler *c = (BinderNetworkHandler *)arg;

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

BinderNetworkHandler::BinderNetworkHandler() {
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

void BinderNetworkHandler::run() {
    pthread_t worker;
    string s;

    /*
    int ret = pthread_create(&worker, NULL, &BinderNetworkHandler::dispatch, (void *)(this));
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

message* get_register_request(char *hostname, char *port, char *name, int *argTypes) {
    message *m = new message;

    unsigned int argTypes_len_in_chars = get_argtypes_len(argTypes) * sizeof(*argTypes);

    int message_len = (
        METADATA_LEN +
        MAX_HOSTNAME_LEN +
        MAX_PORT_LEN +
        MAX_FUNCTION_NAME_LEN +
        argTypes_len_in_chars
    );

    //m->length = message_len;
    //m->type = REGISTER;
    
    m->buf = new char[message_len];

    ((int *)m->buf)[0] = message_len;
    ((int *)m->buf)[1] = REGISTER;


    unsigned int offset  = METADATA_LEN;

    strncpy(m->buf + offset, hostname, MAX_HOSTNAME_LEN);
    offset += MAX_HOSTNAME_LEN;

    strncpy(m->buf + offset, port, MAX_PORT_LEN);
    offset += MAX_PORT_LEN;

    strncpy(m->buf + offset, name, MAX_FUNCTION_NAME_LEN);
    offset += MAX_FUNCTION_NAME_LEN;

    strncpy(m->buf + offset, (char *)argTypes, argTypes_len_in_chars);
    offset += argTypes_len_in_chars;

    assert(message_len == offset);

    return m;
}

int BinderNetworkHandler::rpcRegister(char *name, int *argTypes, skeleton f) {
    unsigned int argTypes_len = get_argtypes_len(argTypes);


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
}

BinderNetworkHandler *b = NULL;

int rpcInit() {
    if (b == NULL) {
        try {
            b = new BinderNetworkHandler();
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
    if (b == NULL) {
      throw UNITIALIZED_BINDER_NETWORK_HANDLER;
    }

    return b->rpcRegister(name, argTypes, f);
}

int rpcExecute() {
    return 0;
}
