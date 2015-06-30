#include "message.h"

#include <cassert>

#include "common_defs.h"

message* get_register_request(char *hostname, char *port, char *name, int *argTypes) {
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

message* get_loc_request(char *name, int *argTypes) {
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

    return m;
}

message *get_loc_success(char *hostname, char *port) {
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

message *get_execute_failure(reason_code reasonCode) {
    message *m = new message;

    int message_len = (
        METADATA_LEN +
        sizeof(reason_code)
    );

    m->buf = new char[message_len];

    ((int *)m->buf)[0] = message_len;
    ((int *)m->buf)[1] = EXECUTE_FAILURE;

    size_t offset = METADATA_LEN;
    memcpy(m->buf + offset, (char *)reasonCode, sizeof(reason_code));

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
