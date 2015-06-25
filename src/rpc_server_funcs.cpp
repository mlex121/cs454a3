#include <iostream>
#include <list>

#include <arpa/inet.h>
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

#include "../include/rpc.h"

#define MAX_DATA_LEN 576 - 4 - 1
struct message {
  int length;
  char s[MAX_DATA_LEN];
};

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

using namespace std;

class ClientNetworkHandler {
  int sock_fd;

  list<string> client_data;

  sem_t read_avail;
  sem_t write_avail;

  struct addrinfo hints, *servinfo, *p;
  int rv;
  char remoteIP[INET6_ADDRSTRLEN];

  public:
    ClientNetworkHandler();
    void run();
    static void *dispatch(void *arg);
};

void* ClientNetworkHandler::dispatch(void *arg) {
  ClientNetworkHandler *c = (ClientNetworkHandler *)arg;

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

ClientNetworkHandler::ClientNetworkHandler() {
  char *hostname  = getenv("SERVER_ADDRESS");
  char *port      = getenv("SERVER_PORT");

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
          perror("client: socket");
          continue;
      }

      if (connect(sock_fd, p->ai_addr, p->ai_addrlen) == -1) {
          close(sock_fd);
          perror("client: connect");
          continue;
      }

      break;
  }

  if (p == NULL) {
      cerr << "client: failed to connect" << endl;
      exit(1);
  }

  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
          remoteIP, sizeof(remoteIP));
  printf("client: connected to %s on port %s\n", remoteIP, port);

  freeaddrinfo(servinfo); // all done with this structure

  sem_init(&read_avail, 0, 0);
  sem_init(&write_avail, 0, 1);
}

void ClientNetworkHandler::run() {
  pthread_t worker;
  string s;

  int ret = pthread_create(&worker, NULL, &ClientNetworkHandler::dispatch, (void *)(this));
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

ClientNetworkHandler c;

int rpcInit() {
  return 0;
}

int rpcRegister(char* name, int* argTypes, skeleton f) {
  return 0;
}

int rpcExecute() {
  return 0;
}
