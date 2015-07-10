#include "server.h"
#include "common_defs.h"

ServerSender* Server::server_sender = NULL;
ServerReceiver* Server::server_receiver= NULL;

void Server::initialize() {
    if (server_receiver && server_sender) {
        throw PREVIOUSLY_INITIALIZED_NETWORK_HANDLERS;
    }

    // TODO is this right?
    // Let partial initializations happen
    if (!server_receiver) server_receiver = new ServerReceiver();
    if (!server_sender) server_sender = new ServerSender(server_receiver->hostname, server_receiver->port);
};

ServerSender* Server::get_sender() {
    if (!server_sender) {
        throw UNINITIALIZED_NETWORK_HANDLERS;
    }

    return server_sender;
};

ServerReceiver* Server::get_receiver() {
    if (!server_receiver) {
        throw UNINITIALIZED_NETWORK_HANDLERS;
    }

    return server_receiver;
};
