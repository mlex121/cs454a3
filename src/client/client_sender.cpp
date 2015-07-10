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
    message *location_request_message = get_loc_request(name, argTypes);
    send_message(location_request_message);

    message *location_reply_message = receive_reply();

    char hostname[MAX_HOSTNAME_LEN];
    char port[MAX_PORT_LEN];
    int offset;

    switch (*((int *)(location_reply_message->buf) + 1)) {
        case LOC_SUCCESS:
            offset = METADATA_LEN;

            strncpy(hostname, location_reply_message->buf + offset, MAX_HOSTNAME_LEN);
            offset += MAX_HOSTNAME_LEN;

            strncpy(port, location_reply_message->buf + offset, MAX_PORT_LEN);
            offset += MAX_PORT_LEN;

            break;
        default:
            cerr << "Message type is: " << *((int *)(location_reply_message->buf) + 1) << endl;
            return 1;
            break;
    }

    cerr << "Hostname is: " << hostname << endl;
    cerr << "Port is: " << port << endl;

    message *execute_request_message = get_execute(EXECUTE, name, argTypes, (const void **)args);

    return 0;
}
