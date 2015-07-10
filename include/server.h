#ifndef SERVER_H
#define SERVER_H

#include "server_receiver.h"
#include "server_sender.h"

class Server {
    Server() {};

    static ServerSender *server_sender;
    static ServerReceiver *server_receiver;

    public:
        static void initialize();
        static ServerSender* get_sender();
        static ServerReceiver* get_receiver();
};

#endif
