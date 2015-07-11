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

ClientSender::ClientSender(char *hostname, char *port) :
    NetworkSender(hostname, port)
{
}


void ClientSender::rpcCall(char *name, int *argTypes, void ** args) {
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
            throw UNRECOGNIZED_MESSAGE_TYPE;
            break;
    }


    delete[] location_reply_message->buf;
    delete location_reply_message;

    //cerr << "Hostname is: " << hostname << endl;
    //cerr << "Port is: " << port << endl;

    message *execute_request_message = get_execute(EXECUTE, name, argTypes, (const void **)args);

    ClientSender client_to_server = ClientSender(hostname, port);
    client_to_server.send_message(execute_request_message);

    message *execute_reply_message = client_to_server.receive_reply();

    char reply_function_name[MAX_FUNCTION_NAME_LEN];
    unsigned int argTypes_bytes;
    unsigned int arg_len; 
    
    switch (*((int *)(execute_reply_message->buf) + 1)) {
        case EXECUTE_SUCCESS:
            offset = METADATA_LEN;

            strncpy(reply_function_name, execute_reply_message->buf + offset, MAX_FUNCTION_NAME_LEN);
            offset += MAX_FUNCTION_NAME_LEN;

            //cerr << "Reply function name we got: " << reply_function_name << endl;

            //Skip over argTypes - we don't need them
            argTypes_bytes = get_argtypes_len(argTypes) * sizeof(*argTypes);
            offset += argTypes_bytes;

            //Reload the args back into the original args array
            while (*argTypes) {
                arg_len = *argTypes & ARG_LEN_MASK;
                if (arg_len == 0) arg_len = 1;
                arg_len *= ARG_SIZES[get_argtype(*argTypes)];

                memcpy(*args, execute_reply_message->buf + offset, arg_len);
                offset += arg_len;

                argTypes++;
                args++;
            }


            break;
        case EXECUTE_FAILURE:
            throw EXECUTION_FAILURE;
            break;
        default:
            cerr << "Message type is: " << *((int *)(location_reply_message->buf) + 1) << endl;
            throw UNRECOGNIZED_MESSAGE_TYPE;
            break;
    }
}

void ClientSender::rpcTerminate() {
    message *termination_message = get_terminate();
    send_message(termination_message);

}
