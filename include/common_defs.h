#ifndef COMMON_DEFS_H
#define COMMON_DEFS_H

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>

#define MAX_HOSTNAME_LEN  256
#define MAX_DATA_LEN 256

enum Type {
  T1,
  T2
};

struct message {
  int length;
  Type type;
  char *s;
};

#endif

