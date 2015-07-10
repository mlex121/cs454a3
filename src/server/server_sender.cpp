#include "common_defs.h"
#include "message.h"
#include "server.h"
#include "server_sender.h"

#include <iostream>

#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
//#include <sys/wait.h>

using namespace std;

ServerSender::ServerSender(char receiver_hostname[MAX_HOSTNAME_LEN], char receiver_port[MAX_PORT_LEN]) :
    NetworkSender(getenv("BINDER_ADDRESS"), getenv("BINDER_PORT"))
{
    strncpy(this->receiver_hostname,    receiver_hostname,  MAX_HOSTNAME_LEN);
    strncpy(this->receiver_port,        receiver_port,      MAX_PORT_LEN);

    sem_init(&read_avail, 0, 0);
    sem_init(&write_avail, 0, 1);
}

void ServerSender::rpcRegister(char *name, int *argTypes, skeleton f) {
    Server::get_receiver()->add_skeleton(name, argTypes, f);

    message *m = get_register_request(receiver_hostname, receiver_port, name, argTypes);

    send_message(m);

    //Receive server reply
}
