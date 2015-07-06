#ifndef SERVER_RECEIVER_H
#define SERVER_RECEIVER_H

#include "receiver.h"

class ServerReceiver : public NetworkReceiver {

    protected:
        virtual void process_message(int fd);
};

#endif
