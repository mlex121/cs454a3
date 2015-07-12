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

// Name conflict on OS X with get_terminate()
// using namespace std;

ClientSender::ClientSender() :
    NetworkSender(getenv("BINDER_ADDRESS"), getenv("BINDER_PORT"))
{
}

ClientSender::ClientSender(const char *hostname, const char *port) :
    NetworkSender(hostname, port)
{
}

int ClientSender::execute(const char *hostname, const char *port, char *name, int *argTypes, void **args) {
    message *execute_request_message = get_execute(EXECUTE, name, argTypes, (const void **)args);

    ClientSender client_to_server = ClientSender(hostname, port);
    client_to_server.send_message(execute_request_message);

    message *execute_reply_message = client_to_server.receive_reply();

    char reply_function_name[MAX_FUNCTION_NAME_LEN];
    unsigned int argTypes_bytes;
    unsigned int arg_len;
    reason_code reason;
    unsigned int offset = METADATA_LEN;

    switch (*((int *)(execute_reply_message->buf) + 1)) {
        case EXECUTE_SUCCESS:

            strncpy(reply_function_name, execute_reply_message->buf + offset, MAX_FUNCTION_NAME_LEN);
            offset += MAX_FUNCTION_NAME_LEN;

            //std::cerr << "Reply function name we got: " << reply_function_name << std::endl;

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

            // break;
            return EXECUTE_SUCCESS;
        case EXECUTE_FAILURE:
            memcpy(&reason, execute_reply_message->buf + METADATA_LEN, sizeof(reason_code));

            switch (reason) {
                case REASON_UNKNOWN_FUNCTION:
                    return FUNCTION_NOT_FOUND;
                    break;
                case REASON_FUNCTION_RETURNED_ERROR:
                    return EXECUTION_FAILURE;
                    break;
                default:
                    return UNKNOWN_REASON;
                    break;
            }
            break;
        default:
            // throw UNRECOGNIZED_MESSAGE_TYPE;
            // break;
            return UNRECOGNIZED_MESSAGE_TYPE;
    }
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
        case LOC_FAILURE:
            throw NO_MATCHING_SERVERS;
        default:
            throw UNRECOGNIZED_MESSAGE_TYPE;
            break;
    }


    delete[] location_reply_message->buf;
    delete location_reply_message;

    //std::cerr << "Hostname is: " << hostname << std::endl;
    //std::cerr << "Port is: " << port << std::endl;

    switch(execute(hostname, port, name, argTypes, args)) {
        case EXECUTE_SUCCESS:
            break;

        // EXECUTE_FAILURE cases
        case FUNCTION_NOT_FOUND:
            throw FUNCTION_NOT_FOUND;
            break;
        case EXECUTION_FAILURE:
            throw EXECUTION_FAILURE;
            break;
        case UNKNOWN_REASON:
            throw UNKNOWN_REASON;
            break;
        default:
            throw UNRECOGNIZED_MESSAGE_TYPE;
            break;
    }
}

void ClientSender::rpcCacheCall(char *name, int *argTypes, void **args) {
    std::string arguments = get_argTypes_string(argTypes);
    CompleteFunction complete_function = std::make_pair(name, arguments);

    if (function_locations.count(complete_function)) {
        std::set<ServerLocation> server_locations = function_locations[complete_function];

        for (
            std::set<ServerLocation>::iterator server_loc_it = server_locations.begin();
            server_loc_it != server_locations.end();
            ++server_loc_it
        )
        {
            const char *hostname = server_loc_it->first.c_str();
            const char *port = server_loc_it->second.c_str();
            int ret_val = execute(hostname, port, name, argTypes, args);
            if (ret_val == EXECUTE_SUCCESS) {
                // We're done, no need to try any other servers
                return;
            }
            else if (ret_val != FUNCTION_NOT_FOUND) {
                throw (ERRORS)ret_val;
            }
            else {
                //Do nothing
            }
        }
    }

    message *cache_request_message = get_cache_loc_request(name, argTypes);
    send_message(cache_request_message);

    message *cache_reply_message = receive_reply();
    char hostname[MAX_HOSTNAME_LEN];
    char port[MAX_PORT_LEN];
    unsigned int message_len = ((int *)cache_reply_message->buf)[0];
    unsigned int offset = METADATA_LEN;

    switch (*((int *)(cache_reply_message->buf) + 1)) {
        case CACHE_LOC_SUCCESS:
            function_locations[complete_function] = {};

            while (offset < message_len) {
                strncpy(hostname, cache_reply_message->buf + offset, MAX_HOSTNAME_LEN);
                offset += MAX_HOSTNAME_LEN;

                strncpy(port, cache_reply_message->buf + offset, MAX_PORT_LEN);
                offset += MAX_PORT_LEN;

                ServerLocation server_location = std::make_pair(hostname, port);
                function_locations[complete_function].insert(server_location);
            }

            break;
        case CACHE_LOC_FAILURE:
            throw FUNCTION_NOT_FOUND;
            // Without any cached locations we don't have more to do.
            return;
        default:
            //std::cerr << "Message type is: " << *((int *)(cache_reply_message->buf) + 1) << std::endl;
            throw UNRECOGNIZED_MESSAGE_TYPE;
            // Without any cached locations we don't have more to do.
            return;
    }

    std::set<ServerLocation> server_locations = function_locations[complete_function];

    for (
        std::set<ServerLocation>::iterator server_loc_it = server_locations.begin();
        server_loc_it != server_locations.end();
        ++server_loc_it
    )
    {
        const char *hostname = server_loc_it->first.c_str();
        const char *port = server_loc_it->second.c_str();
        int ret_val = execute(hostname, port, name, argTypes, args);
        if (ret_val == EXECUTE_SUCCESS) {
            // We're done, no need to try any other servers
            return;
        }
        else if (ret_val != FUNCTION_NOT_FOUND) {
            throw (ERRORS)ret_val;
        }
        else {
            //Do nothing
        }
    }
}

void ClientSender::rpcTerminate() {
    message *termination_message = get_terminate();
    send_message(termination_message);
}
