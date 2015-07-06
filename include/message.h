#ifndef MESSAGE_H
#define MESSAGE_H

#include "common_defs.h"

message *get_register_request(char *hostname, char *port, char *name, int *argTypes);

message *get_loc_request(char *name, int *argTypes);
message *get_loc_success(char *hostname, char *port);
message *get_loc_failure(reason_code reasonCode);
// message *get_execute_request(char *name, int *argTypes, );
// message *get_execute_success(char *name, int *argTypes, );
message *get_execute_failure(reason_code reasonCode);
message *get_terminate();

#endif // MESSAGE_H
