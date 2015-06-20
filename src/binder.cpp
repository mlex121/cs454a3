#include <iostream>

#include <arpa/inet.h>
#include <ctype.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../include/common_defs.h"

#define MAX_CONNECTIONS  10 

using namespace std;

void socket_setup(int &sock, struct sockaddr_in &addr) {

    if((sock=socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        cerr << "Unable to create socket" << endl;
        exit(1);
    }

    //Initialize the sockaddr_in fields to 0 for our server address
    memset(&addr, 0, sizeof(struct sockaddr_in));

    //Miscellaneous address settings
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    //We take any assigned port
    addr.sin_port = htons(0);

    //Bind to a socket
    if(bind(sock, (struct sockaddr *) &addr, sizeof(struct sockaddr_in)) < 0)
    {
        cerr << "Unable to bind to socket" << endl;
        exit(1);
    }

}

class NetworkHandler {
  int listen_fd;
  int new_fd;
  int max_fd;

  struct sockaddr_in receiver_addr, addr_info;
  struct sockaddr_storage sender_addr;

  fd_set master_fds;
  fd_set current_fds;

  socklen_t sock_len;
  socklen_t sender_sock_len;

  message *m;

  void handle_set_fd(int fd);

  public:
    NetworkHandler();
    void run();
};

NetworkHandler::NetworkHandler() {
  sock_len =          sizeof(struct sockaddr);
  sender_sock_len =  sizeof(struct sockaddr_storage);

  m = new message;

  //Setup a socket, and bind our address to it
  socket_setup(listen_fd, receiver_addr);

  //Print our channel info
  char hostname[MAX_HOSTNAME_LEN];
  gethostname(hostname, MAX_HOSTNAME_LEN);
  getsockname(listen_fd, (struct sockaddr *)&addr_info, &sock_len);
  cout << "SERVER_ADDRESS " << hostname << endl;
  cout << "SERVER_PORT " << ntohs(addr_info.sin_port) << endl;

  //Begin listening on the socket
  if (listen(listen_fd, MAX_CONNECTIONS) == -1) {
    perror("listen");
    exit(1);
  }

  // Set our highest seen fd to listener fd
  max_fd = listen_fd;
  
  // Set the master list of file descriptors to the listener socket
  FD_ZERO(&master_fds);
  FD_SET(listen_fd, &master_fds);
}

void NetworkHandler::run() {
  while(true) {  
    current_fds = master_fds; // copy it
    if (select(max_fd+1, &current_fds, NULL, NULL, NULL) == -1) {
      //perror("select");
      exit(4);
    }

    for(int i = 0; i <= max_fd; i++) {
      if (FD_ISSET(i, &current_fds)) {
        handle_set_fd(i);
      }
    }
  }
}

void NetworkHandler::handle_set_fd(int fd) {
  // handle new connections to the main listener socket
  if (fd == listen_fd) {
    new_fd = accept(
      listen_fd,
      (struct sockaddr *)&sender_addr,
      &sender_sock_len
    );

    if (new_fd != -1) {
        FD_SET(new_fd, &master_fds); // add to master set
        if (new_fd > max_fd) {
            max_fd = new_fd;
        }

        /*
        char remoteIP[INET6_ADDRSTRLEN];
        printf(
          "selectserver: new connection from %s on socket %d\n",
          inet_ntop(
            sender_addr.ss_family,
            get_in_addr((struct sockaddr*)&sender_addr),
            remoteIP,
            INET6_ADDRSTRLEN
          ),
          new_fd
        );
        */
    }
    else {
        //perror("accept");
    }
  }
  // Data is from a client
  else {
    char buf[MAX_DATA_LEN];
    int size;

    // Error or connection closed by client
    if ((size = recv(fd, m->s, MAX_DATA_LEN, 0)) <= 0) {

      if (size == 0) {
        //cout << "Socket " << fd << " hung up." << endl;
      }
      else {
        perror("recv");
      }

      //Remove client from the master fd list
      close(fd);
      FD_CLR(fd, &master_fds);
    }
    else {
      // handle client data

      // Add on the extra null character here
      m->s[size] = '\0';
      size++;

      m->length = size;

      //cout << "Sending back the string: " << m->s << endl;

      //Send the client back the title case string
      if (send(fd, (char*)m, size+4, 0) == -1) {
        //perror("send");
      }
    }
  } 
}

int main(int argc, char **argv) {
  NetworkHandler n; 

  n.run();

  return 0;
}
