
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

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
            server_sender = new ServerSender();
            server_receiver = new ServerReceiver();
        }
        catch (SERVER_ERRORS e) {
            return e;
        }
    }
    else if (server_receiver && server_sender) {
        return ALREADY_INITIALIZED_NETWORK_HANDLERS;
    }
    else {
        UNITIALIZED_NETWORK_HANDLERS;
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

    return 0;
}
