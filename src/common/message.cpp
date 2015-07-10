#include <iostream>

#include <cassert>
#include <string.h>

#include "common_defs.h"
#include "message.h"

using  namespace std;

message* get_register_request(const char *hostname, const char *port, const char *name, const int *argTypes) {
    message *m = new message;

    unsigned int argTypes_bytes = get_argtypes_len(argTypes) * sizeof(*argTypes);

    int message_len = (
        METADATA_LEN +
        MAX_HOSTNAME_LEN +
        MAX_PORT_LEN +
        MAX_FUNCTION_NAME_LEN +
        argTypes_bytes
    );

    m->buf = new char[message_len];

    ((int *)m->buf)[0] = message_len;
    ((int *)m->buf)[1] = REGISTER;

    unsigned int offset  = METADATA_LEN;

    strncpy(m->buf + offset, hostname, MAX_HOSTNAME_LEN);
    offset += MAX_HOSTNAME_LEN;

    strncpy(m->buf + offset, port, MAX_PORT_LEN);
    offset += MAX_PORT_LEN;

    strncpy(m->buf + offset, name, MAX_FUNCTION_NAME_LEN);
    offset += MAX_FUNCTION_NAME_LEN;

    memcpy(m->buf + offset, (char *)argTypes, argTypes_bytes);
    offset += argTypes_bytes;

    assert(message_len == offset);

    return m;
}

message* get_loc_request(const char *name, const int *argTypes) {
    message *m = new message;

    size_t argTypes_bytes = get_argtypes_len(argTypes) * sizeof(*argTypes);

    int message_len = (
        METADATA_LEN +
        MAX_FUNCTION_NAME_LEN +
        argTypes_bytes
    );

    m->buf = new char[message_len];

    ((int *)m->buf)[0] = message_len;
    ((int *)m->buf)[1] = LOC_REQUEST;

    unsigned int offset = METADATA_LEN;

    strncpy(m->buf + offset, name, MAX_FUNCTION_NAME_LEN);
    offset += MAX_FUNCTION_NAME_LEN;

    memcpy(m->buf + offset, (char *)argTypes, argTypes_bytes);
    offset += argTypes_bytes;

    assert(message_len == offset);

    /*
    cerr << "Message length is: " << message_len << endl;
    cerr << "Message type is: " << LOC_REQUEST << endl;
    */

    return m;
}

message *get_loc_success(const char *hostname, const char *port) {
    message *m = new message;

    int message_len = (
        METADATA_LEN +
        MAX_HOSTNAME_LEN +
        MAX_PORT_LEN
    );

    m->buf = new char[message_len];

    ((int *)m->buf)[0] = message_len;
    ((int *)m->buf)[1] = LOC_SUCCESS;

    unsigned int offset = METADATA_LEN;

    strncpy(m->buf + offset, hostname, MAX_HOSTNAME_LEN);
    offset += MAX_HOSTNAME_LEN;

    strncpy(m->buf + offset, port, MAX_PORT_LEN);
    offset += MAX_PORT_LEN;

    assert(message_len == offset);

    return m;
}

message *get_loc_failure(reason_code reasonCode) {
    message *m = new message;

    int message_len = (
        METADATA_LEN +
        sizeof(reason_code)
    );

    m->buf = new char[message_len];

    ((int *)m->buf)[0] = message_len;
    ((int *)m->buf)[1] = LOC_FAILURE;

    size_t offset = METADATA_LEN;
    memcpy(m->buf + offset, (char *)reasonCode, sizeof(reason_code));

    assert(message_len == offset);

    return m;
}

message *get_execute(const message_type m_type, const char *name, const int *argTypes, const void **args) {
    //cerr << "Reply function name is: " << name << endl;
    message *m = new message;

    unsigned int argTypes_bytes = get_argtypes_len(argTypes) * sizeof(*argTypes);
    unsigned int args_bytes = get_args_len(argTypes);

    int message_len = (
        METADATA_LEN +
        MAX_FUNCTION_NAME_LEN +
        argTypes_bytes + 
        args_bytes
    );

    m->buf = new char[message_len];

    ((int *)m->buf)[0] = message_len;
    ((int *)m->buf)[1] = m_type;

    unsigned int offset  = METADATA_LEN;

    strncpy(m->buf + offset, name, MAX_FUNCTION_NAME_LEN);
    offset += MAX_FUNCTION_NAME_LEN;

    memcpy(m->buf + offset, (char *)argTypes, argTypes_bytes);
    offset += argTypes_bytes;

    while (*argTypes) {
        unsigned int arg_len = *argTypes & ARG_LEN_MASK; 
        if (arg_len == 0) arg_len = 1;
        arg_len *= ARG_SIZES[get_argtype(*argTypes)];

        /*
        cerr << "Argtype is: " << ARG_NAMES[get_argtype(*argTypes)];
        cerr << " args_len is: " << arg_len;
        cerr << endl;
        */

        memcpy(m->buf + offset, (char *)(*args), arg_len);
        offset += arg_len;

        argTypes++;
        args++;
    }

    assert(message_len == offset);

    return m;
}

message *get_execute_failure(reason_code reasonCode) {
    message *m = new message;

    int message_len = (
        METADATA_LEN +
        sizeof(reason_code)
    );

    m->buf = new char[message_len];

    ((int *)m->buf)[0] = message_len;
    ((int *)m->buf)[1] = EXECUTE_FAILURE;

    unsigned int offset = METADATA_LEN;
    memcpy(m->buf + offset, (char *)&reasonCode, sizeof(reason_code));
    offset += sizeof(reason_code);

    assert(message_len == offset);

    return m;
}

message *get_terminate() {
    message *m = new message;
    int message_len = METADATA_LEN;

    m->buf = new char[message_len];

    ((int *)m->buf)[0] = message_len;
    ((int *)m->buf)[1] = TERMINATE;

    return m;
}
