#ifndef COMMON_DEFS_H
#define COMMON_DEFS_H

#include <string>
#include <utility>

#include "rpc.h"

// Message Handling

// We allow up to one extra character, so that null characters will
// be copied if a string is the maximum length
#define PADDING 4

#define MAX_HOSTNAME_LEN      (64 + PADDING)
#define MAX_PORT_LEN          (64 + PADDING)
#define MAX_FUNCTION_NAME_LEN (64 + PADDING)

// Size of the metadata at the start of a message
#define METADATA_LEN 8

// Maximum size for a single send request
// this is large, because we want to minimize network traffic
#define MAX_SEND_SIZE 4096

#define ARG_INPUT_MASK (1 << ARG_INPUT)
#define ARG_OUTPUT_MASK (1 << ARG_OUTPUT)

#define ARG_TYPE_MASK 0x00FF0000
#define ARG_LEN_MASK  0x0000FFFF

extern int ARG_SIZES[];
extern char const *ARG_NAMES[];

// Fully qualified with type arguments, used in binder and server databases
typedef std::pair<std::string, std::string> CompleteFunction;

enum message_type {
    REGISTER,
    REGISTER_SUCCESS,
    REGISTER_FAILURE,

    LOC_REQUEST,
    LOC_SUCCESS,
    LOC_FAILURE,

    EXECUTE,
    EXECUTE_SUCCESS,
    EXECUTE_FAILURE,

    TERMINATE
};

enum reason_code {
    //Binder
    REASON_NO_MATCHING_SERVERS,
    REGISTRATION_SUCCESSFUL,

    //Server
    REASON_UNKNOWN_FUNCTION,
    REASON_FUNCTION_RETURNED_ERROR,
};

struct message {
    // Buf contains length followed by type, followed by message
    char *buf;
};


#define NUM_ERRORS 100

enum ERRORS {
    //General
    NETWORK_ERROR = -1*NUM_ERRORS,
    UNRECOGNIZED_MESSAGE_TYPE,

    //Binder
    // The binder should not throw any errors

    //Server
    UNINITIALIZED_NETWORK_HANDLERS,
    REGISTRATION_AFTER_INITIALIZATION,

    //Client
    NO_MATCHING_SERVERS,
    FUNCTION_NOT_FOUND,
    EXECUTION_FAILURE,
    UNKNOWN_REASON,

    //Warnings start here
    
    //Server
    FUNCTION_REREGISTRATION = 0,
    PREVIOUSLY_INITIALIZED_NETWORK_HANDLERS = 1,
    DUPLICATE_FUNCTION_REGISTRATION,
};

unsigned int get_argtypes_len(const int *argTypes);
unsigned int get_argtype(const int argType);
unsigned int get_args_len(const int *argTypes);
std::string get_argTypes_string(const int *argTypes);

#endif
