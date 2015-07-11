#include <pthread.h>
#include <signal.h>

#include "common_defs.h"
#include "rpc.h"
#include "server.h"

using namespace std;

int rpcInit() {
    try {
        Server::initialize();
    }
    catch (ERRORS e) {
        return e;
    }

    return 0;
}

int rpcRegister(char* name, int* argTypes, skeleton f) {
    try {
        Server::get_sender()->rpcRegister(name, argTypes, f);
    }
    catch (ERRORS e) {
        return e;
    }

    return 0;
}

int rpcExecute() {
    try {
        Server::get_sender()->await_termination();
        Server::get_receiver()->rpcInit();
    }
    catch (ERRORS e) {
        return e;
    }

    return 0;
}
