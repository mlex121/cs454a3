#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rpc.h"

#define CHAR_ARRAY_LENGTH 100

int main() {
    int a0 = 5;
    int b0 = 10;
    int count0 = 3;
    int return0;
    int argTypes0[count0 + 1];
    void **args0;

    argTypes0[0] = (1 << ARG_OUTPUT) | (ARG_INT << 16);
    argTypes0[1] = (1 << ARG_INPUT) | (ARG_INT << 16);
    argTypes0[2] = (1 << ARG_INPUT) | (ARG_INT << 16);
    argTypes0[3] = 0;
        
    args0 = (void **)malloc(count0 * sizeof(void *));
    args0[0] = (void *)&return0;
    args0[1] = (void *)&a0;
    args0[2] = (void *)&b0;

    // rpcCalls
    int s0 = rpcCall("f0", argTypes0, args0);
    // test the return f0
    printf("\nEXPECTED return of f0 is: %d\n", a0 + b0);
    if (s0 >= 0) { 
        printf("ACTUAL return of f0 is: %d\n", *((int *)(args0[0])));
    }
    else {
        printf("Error: %d\n", s0);
    }

    rpcTerminate();

    return 0;
}




