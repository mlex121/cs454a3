#include <iostream>

#include "rpc.h"
#include "client_sender.h"

using namespace std;

ClientSender *client_sender = NULL;

int rpcCall(char* name, int* argTypes, void** args) {
    try {
        if (!client_sender) client_sender = new ClientSender();
        return client_sender->rpcCall(name, argTypes, args);
    }
    catch (ERRORS e) {
        return e;
    }
}

int rpcTerminate() {
    return 1;
}
