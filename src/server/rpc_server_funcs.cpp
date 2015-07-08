#include <pthread.h>
#include <signal.h>

#include "rpc.h"
#include "server_receiver.h"
#include "server_sender.h"

using namespace std;

ServerSender *server_sender = NULL;
ServerReceiver *server_receiver = NULL;

void check_network_handlers() {
    if (!server_receiver || !server_receiver) {
      throw UNITIALIZED_NETWORK_HANDLERS;
    }
}

int rpcInit() {
    int ret_val = 0;

    if (!server_receiver && !server_sender) {
        try {
            server_receiver = new ServerReceiver();
            server_sender = new ServerSender(server_receiver->hostname, server_receiver->port);
        }
        catch (SERVER_ERRORS e) {
            return e;
        }
    }
    else if (server_receiver && server_sender) {
        return ALREADY_INITIALIZED_NETWORK_HANDLERS;
    }
    else {
        return UNITIALIZED_NETWORK_HANDLERS;
    }

    return 0;
}

int rpcRegister(char* name, int* argTypes, skeleton f) {
    try {
        check_network_handlers();
        return server_sender->rpcRegister(name, argTypes, f);
    }
    catch (SERVER_ERRORS e) {
        return e;
    }
}

int rpcExecute() {
    try {
        check_network_handlers();
        //return server_receiver->rpcExecute();
    }
    catch (SERVER_ERRORS e) {
        return e;
    }

    return 0;
}
