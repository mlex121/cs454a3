#include "rpc.h"

int main(int argc, char *argv[]) {
    
    // create sockets and connect to the binder */
    rpcInit();

    // prepare server functions' signatures */
    int count = 10000;

    int argTypes[count + 1];

    int i;
    for (i=0; i < count; i++) {
        argTypes[i] = (1 << ARG_OUTPUT) | (ARG_INT << 16);
    }

    argTypes[10000] = 0;

    // register server functions f0~f4
    rpcRegister("f0", argTypes, 0);

    // call rpcExecute
    rpcExecute();

    // return
    return 0;
}







