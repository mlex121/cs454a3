//For debugging purposes only
//#include <iostream>

#include <string>

#include "common_defs.h"

using namespace std;

int ARG_SIZES[] = {
    0, //padding
    sizeof(char),
    sizeof(short),
    sizeof(int),
    sizeof(long),
    sizeof(double),
    sizeof(float)
};

// This lets us pre-allocated a string
// we get the value from the length of "double" + " ," + "io" + "*"
#define MAX_ARG_DESCRIPTOR_LENGTH 11

char const *ARG_NAMES[] = {
    "",
    "char",
    "short",
    "int",
    "long",
    "double",
    "float"
};

unsigned int get_argtypes_len(int *argTypes) {
    // We want to include the trailing zero in the count
    unsigned int len = 1;

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


#include <iostream>
string get_argTypes_string(int *argTypes) {
    string argTypes_string;
    argTypes_string.reserve(get_argtypes_len(argTypes) * MAX_ARG_DESCRIPTOR_LENGTH);

    while (*argTypes) {
        argTypes_string += ARG_NAMES[get_argtype(*argTypes)];

        unsigned int arg_len = *argTypes & ARG_LEN_MASK;
        if (arg_len) argTypes_string += '*';

        argTypes_string += ", ";
        argTypes++;
    }

    argTypes_string.pop_back();
    argTypes_string.pop_back();

    return argTypes_string;
}


// Argument Handling
