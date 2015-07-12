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
// we get the value from the length of "double" + " ," + ":i:o" + "*"
#define MAX_ARG_DESCRIPTOR_LENGTH 13

char const *ARG_NAMES[] = {
    "",
    "char",
    "short",
    "int",
    "long",
    "double",
    "float"
};

unsigned int get_argtypes_len(const int *argTypes) {
    // We want to include the trailing zero in the count
    unsigned int len = 1;

    while (*argTypes++) len++;

    return len;
}

unsigned int get_argtype(const int argType) {
    return (argType & ARG_TYPE_MASK) >> 16;
}

unsigned int get_args_len(const int *argTypes) {
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


string get_argTypes_string(const int *argTypes) {
    string argTypes_string;
    argTypes_string.reserve(get_argtypes_len(argTypes) * MAX_ARG_DESCRIPTOR_LENGTH);

    while (*argTypes) {
        argTypes_string += ARG_NAMES[get_argtype(*argTypes)];

        if (*argTypes & ARG_LEN_MASK) argTypes_string += '*';
        if (*argTypes & ARG_INPUT_MASK) argTypes_string += ":i";
        if (*argTypes & ARG_OUTPUT_MASK) argTypes_string += ":o";

        if (*(argTypes+1)) argTypes_string += ", ";
        argTypes++;
    }

    return argTypes_string;
}


// Argument Handling
