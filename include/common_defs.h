#ifndef COMMON_DEFS_H
#define COMMON_DEFS_H

// Message Handling

// We allow up to one extra character, so that null characters will
// be copied if a string is the maximum length
#define PADDING 1

#define MAX_HOSTNAME_LEN      (64 + PADDING)
#define MAX_PORT_LEN          (64 + PADDING)
#define MAX_FUNCTION_NAME_LEN (64 + PADDING)

// Size of the metadata at the start of a message
#define METADATA_LEN 8

// Maximum size for a single send request
// this is large, because we want to minimize network traffic
#define MAX_SEND_SIZE 4096

#define ARG_TYPE_MASK 0x00FF0000
#define ARG_LEN_MASK  0x0000FFFF

extern int ARG_SIZES[];

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

struct message {
    // I'm not actually sure how necessary these are, but have
    // left message as a struct in case we need them, or anything else 
    //int length;
    //message_type type;
    // Buf contains length followed by type, followed by message
    char *buf;
};

unsigned int get_argtypes_len(int *argTypes);
unsigned int get_argtype(int argType);
unsigned int get_args_len(int *argTypes);

#endif
