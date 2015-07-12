#include "common_defs.h"
#include "message.h"
#include "server.h"
#include "server_sender.h"

#include <cassert>
#include <iostream>

#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

using namespace std;

ServerSender::ServerSender(char receiver_hostname[MAX_HOSTNAME_LEN], char receiver_port[MAX_PORT_LEN]) :
    NetworkSender(getenv("BINDER_ADDRESS"), getenv("BINDER_PORT"))
{
    strncpy(this->receiver_hostname,    receiver_hostname,  MAX_HOSTNAME_LEN);
    strncpy(this->receiver_port,        receiver_port,      MAX_PORT_LEN); 
}

void ServerSender::rpcRegister(char *name, int *argTypes, skeleton f) {
    // This may throw a warning if a duplicate function 
    // is already registered which means there is no need to contact the binder
    Server::get_receiver()->add_skeleton(name, argTypes, f);

    message *m = get_register_request(receiver_hostname, receiver_port, name, argTypes);

    send_message(m);

    //Receive server reply
    // FIXME this is important
}

void term(int signum) {
    exit(0);

}

void ServerSender::await_termination() {

    // Setup a termination handler for the parent process
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = term;
    sigaction(SIGTERM, &action, NULL);

    pid_t pID = fork();

    if (!pID) {
        message *m = receive_reply();

        // The only message we should receive from the binder after 
        // we have finished registering is a terminate message
        assert(*((int *)(m->buf) + 1) == TERMINATE);

        kill(getppid(), SIGTERM);
        exit(0);
    }
}
