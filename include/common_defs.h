#ifndef COMMON_DEFS_H
#define COMMON_DEFS_H

// Message Handling

#define MAX_HOSTNAME_LEN      64
#define MAX_FUNCTION_NAME_LEN 64

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
  int length;
  message_type type;
  char *buf;
};

unsigned int get_argtypes_len(int *argTypes);
unsigned int get_argtype(int argType);
unsigned int get_args_len(int *argTypes);

#endif
