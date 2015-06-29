//For debugging purposes only
#include <iostream>
using namespace std;

#include "common_defs.h"

int ARG_SIZES[] = {
    0, //padding
    sizeof(char),
    sizeof(short), 
    sizeof(int), 
    sizeof(long),
    sizeof(double),
    sizeof(float)
};

unsigned int get_argtypes_len(int *argTypes) {
    unsigned int len = 0;

    while (*argTypes++) len++;

    return len;
}

unsigned int get_argtype(int argType) {
    return (argType & ARG_TYPE_MASK) >> 16;
}

unsigned int get_args_len(int *argTypes) {
    unsigned int len = 0;

    while (*argTypes) {
        /*
        cout << "Argtype is: " << (get_argtype(*argTypes));
        cout << " Size is: " << (ARG_SIZES[get_argtype(*argTypes)]);
        cout << " Len is: " << (*argTypes & ARG_LEN_MASK);
        cout << endl;
        */

        unsigned int arg_len = *argTypes & ARG_LEN_MASK;
        if (arg_len == 0) arg_len = 1;

        len += arg_len * ARG_SIZES[get_argtype(*argTypes)];

        argTypes++;
    }
    
    return len;
}


// Argument Handling
