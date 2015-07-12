#include <iostream>

#include "rpc.h"
#include "client_sender.h"

using namespace std;

ClientSender *client_sender = NULL;

int rpcCall(char* name, int* argTypes, void** args) {
    try {
        if (!client_sender) client_sender = new ClientSender();
        client_sender->rpcCall(name, argTypes, args);
    }
    catch (ERRORS e) {
        return e;
    }

    return 0;
}

int rpcCacheCall(char* name, int* argTypes, void** args) {
    try {
        if (!client_sender) client_sender = new ClientSender();
        client_sender->rpcCacheCall(name, argTypes, args);
    }
    catch (ERRORS e) {
        return e;
    }

    return 0;
}

int rpcTerminate() {
    try {
        if (!client_sender) client_sender = new ClientSender();
        client_sender->rpcTerminate();
    }
    catch (ERRORS e) {
        return e;
    }

    return 0;
}
