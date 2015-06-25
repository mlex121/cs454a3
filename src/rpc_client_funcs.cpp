#include <iostream>

#include <arpa/inet.h>
#include <ctype.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../include/rpc.h"

using namespace std;

int rpcCall(char* name, int* argTypes, void** args) {
    return 0;
}

int rpcTerminate() {
    return 1;
}
