#include <iostream> //FIXME debugging only

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
        // FIXME get the sender to start receiving as well for terminate?
        // actually, what is supposed to happen for terminate exactly? 
        // are the server supposed to keep open connections to the binder?
        // 5 hosts max seems awfully low for this?????
        Server::get_receiver()->run();
    }
    catch (ERRORS e) {
        return e;
    }

    return 0;
}
