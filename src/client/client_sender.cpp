#include "common_defs.h"
#include "message.h"
#include "client_sender.h"

#include <iostream>

#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
//#include <sys/wait.h>

using namespace std;

ClientSender::ClientSender() :
    NetworkSender(getenv("BINDER_ADDRESS"), getenv("BINDER_PORT"))
{
}


int ClientSender::rpcCall(char *name, int *argTypes, void ** args) {
    message *m = get_loc_request(name, argTypes);

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
