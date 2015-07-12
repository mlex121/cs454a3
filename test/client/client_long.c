
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "rpc.h"

// Maximum size for any argument is 2^16 - 1
#define ARRAY_SIZE 60000 //65535

int main() {
    srand(time(NULL));

    // prepare the arguments for f3
    long *a3 = (long *)malloc(sizeof(long) * ARRAY_SIZE);
    int count3 = 1;
    int argTypes3[count3 + 1];
    void **args3;

    int j;
    for (j = 0; j < ARRAY_SIZE; j++) {
        int num = rand() % 1000;
        a3[j] = num;
    }

    a3[0] = ARRAY_SIZE;

    argTypes3[0] = (1 << ARG_OUTPUT) | (1 << ARG_INPUT) | (ARG_LONG << 16) | ARRAY_SIZE;
    argTypes3[1] = 0;

    args3 = (void **)malloc(count3 * sizeof(void *));
    args3[0] = (void *)a3;

    int s3 = rpcCall("f3", argTypes3, args3);

    printf(
        "\nEXPECTED return of f3 is: a sorted list, decreasing\n"
    );
    printf("ACTUAL return of f3 is: %d\n", s3);
    int i;
    for (i = 0; i < 10; i++) {
        printf(" %ld", *(((long *)args3[0]) + i));
    }
    printf("\n");

    rpcTerminate();

    return 0;
}




