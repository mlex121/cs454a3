#ifndef MESSAGE_H
#define MESSAGE_H

#include "common_defs.h"

message* get_register_request(const char *hostname, const char *port, const char *name, const int *argTypes);
message *get_register_success(reason_code reasonCode);

message* get_loc_request(const char *name, const int *argTypes);
message *get_loc_success(const char *hostname, const char *port);
message *get_loc_failure(reason_code reasonCode);

message *get_cache_loc_request(const char *name, const int *argTypes);
message *get_cache_loc_success(std::set<ServerLocation> locations);
message *get_cache_loc_failure(reason_code reasonCode);

message *get_execute(const message_type, const char *name, const int *argTypes, const void **args);
message *get_execute_failure(reason_code reasonCode);

message *get_terminate();

#endif // MESSAGE_H
